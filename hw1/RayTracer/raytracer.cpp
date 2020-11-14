#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <math.h>
#include <cfloat>


using namespace std;
using namespace parser;
typedef unsigned char RGB[3];

double dot(const Vec3f &a, const Vec3f &b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

struct Color
{
    double r, g, b;
    Color() { r = g = b = 0; }
    Color(double i, double j, double k)
    {
        r = i;
        g = j, b = k;
    }
};

double intersectSphere(const Ray &ray, Sphere &sphere, vector<Vec3f> &my_vertices)
{
    Vec3f centerOfSphere = my_vertices[sphere.center_vertex_id -1];
    float radiusOfSphere = sphere.radius;

    const Vec3f rayOrigin = ray.o;
    const Vec3f rayDirection = ray.d;
    Vec3f originToCenter;

    originToCenter.x = rayOrigin.x - centerOfSphere.x;
    originToCenter.y = rayOrigin.y - centerOfSphere.y;
    originToCenter.z = rayOrigin.z - centerOfSphere.z;

    const double b = 2*dot(rayDirection, originToCenter);
    const double a = dot(rayDirection, rayDirection);
    const double c = dot(originToCenter, originToCenter) - radiusOfSphere * radiusOfSphere;

    double delta = b * b - 4 * a * c;
    double t;
    double t0, t1;

    if (delta < -1){
        return false;
    }
    else if(delta == 0){
        t = -b /(2*a);
    }
    else{
        delta = sqrt(delta);
        t0 = (-b - delta)/(2*a);
        t1 = (-b + delta)/(2*a);
        t = (t0 < t1) ? t0 : t1;
    }
    return t;
}

Ray getRay(Camera camera, int row, int col){

    Vec3f v = camera.up;
    Vec3f gaze = camera.gaze;
    Vec3f u = gaze.cross(v);

    float l = camera.near_plane.x;
    float r = camera.near_plane.y;
    float b = camera.near_plane.z;
    float t = camera.near_plane.w;

    Vec3f m = camera.position + camera.gaze * camera.near_distance;
    Vec3f q = m + u*l + v*t;

    double s_u = (row + 0.5) * (r-l) / camera.image_width;
    double s_v = (col + 0.5) * (t-b)/camera.image_height;

    Vec3f s = q + u*s_u - v*s_v;

    Ray myRay(camera.position, s - camera.position);

    return myRay;
}

double intersectTriangle(Ray &r, Triangle &triangle, vector<Vec3f> &my_vertices) {

//    Vec3f a = my_vertices[triangle.indices.v0_id];
//    Vec3f b = my_vertices[triangle.indices.v1_id];
//    Vec3f c = my_vertices[triangle.indices.v2_id];
//
//    Vec3f cb = c-b;
//    Vec3f ba = b-a;
//    Vec3f ac = a-c;

    double  a,b,c,d,e,f,g,h,i,j,k,l;
    double beta,gamma,t;

    double eimhf,gfmdi,dhmeg,akmjb,jcmal,blmkc;

    double M;

    double dd;
    Vec3f ma,mb,mc;

    ma = my_vertices[triangle.indices.v0_id-1];
    mb = my_vertices[triangle.indices.v1_id-1];
    mc = my_vertices[triangle.indices.v2_id-1];

    a = ma.x-mb.x;
    b = ma.y-mb.y;
    c = ma.z-mb.z;

    d = ma.x-mc.x;
    e = ma.y-mc.y;
    f = ma.z-mc.z;

    g = r.d.x;
    h = r.d.y;
    i = r.d.z;

    j = ma.x-r.o.x;
    k = ma.y-r.o.y;
    l = ma.z-r.o.z;

    eimhf = e*i-h*f;
    gfmdi = g*f-d*i;
    dhmeg = d*h-e*g;
    akmjb = a*k-j*b;
    jcmal = j*c-a*l;
    blmkc = b*l-k*c;

    M = a*eimhf+b*gfmdi+c*dhmeg;
    if (M==0) return -1;

    t = -(f*akmjb+e*jcmal+d*blmkc)/M;

    if (t<1.0) return -1;

    gamma = (i*akmjb+h*jcmal+g*blmkc)/M;

    if (gamma<0 || gamma>1) return -1;

    beta = (j*eimhf+k*gfmdi+l*dhmeg)/M;

    if (beta<0 || beta>(1-gamma)) return -1;

    return t;
}

int main(int argc, char* argv[])
{
    parser::Scene scene;
    scene.loadFromXml(argv[1]);

    vector<Camera> my_cameras =  scene.cameras;
    vector<Sphere> my_spheres =  scene.spheres;
    vector<Vec3f> my_vertices = scene.vertex_data;
    vector<Triangle> my_triangles = scene.triangles;
    vector<Mesh> my_meshes = scene.meshes;
    vector<Material> my_materials = scene.materials;
    Vec3i backColor = scene.background_color;
    vector<PointLight> my_pointlights = scene.point_lights;
    Vec3f ambientLight = scene.ambient_light;

    for(parser::Camera camera : my_cameras ) {

        int width = camera.image_width, height = camera.image_height;
        unsigned char *image = new unsigned char[width * height * 3];

        int i = 0;
        Triangle myTriangle;


        Material myMaterial;
        Color myColor;
        int materialid;
        Vec3f ambientReflectance;
        Vec3f diffuseReflectance;
        Vec3f specularReflectance;
        float phongExp;
        Vec3f normal;

        Vec3f normalLightVector;
        Vec3f notNormalLightVector;
        double notNormalLightVectorLength;

        Vec3f ViewVector;
        double ViewVectorLength;

        double katsayi;
        Vec3f halfVectorForSpecular;

        Vec3f normalCross;
        Vec3f intensityForDistance;
        Vec3f intersectionPoint;
        int whichObj=0;
        Triangle kesisenTriangle;
        int r,g,b;

        double shadowRayEpsilon = scene.shadow_ray_epsilon;

//        for(Triangle triangleToBeAdjusted : my_triangles ){
//            Vec3f tempNormal = (my_vertices[triangleToBeAdjusted.indices.v2_id - 1] - my_vertices[triangleToBeAdjusted.indices.v1_id - 1]).cross(my_vertices[triangleToBeAdjusted.indices.v0_id - 1] -
//                    my_vertices[triangleToBeAdjusted.indices.v1_id - 1]);
//            triangleToBeAdjusted.normalVec = tempNormal.normalization();
//
//            cout<< triangleToBeAdjusted.normalVec.x << " " << triangleToBeAdjusted.normalVec.y << " " << triangleToBeAdjusted.normalVec.z << " " <<endl;
//        }
//        for(int s=0; s< my_meshes.size() ; s++){
//
//            for(Face face : my_meshes[s].faces){
//
//                myTriangle.indices = face;
//                Vec3f tempNormal = (my_vertices[face.v2_id - 1] - my_vertices[face.v1_id - 1]).cross(my_vertices[face.v0_id - 1] -
//                        my_vertices[face.v1_id - 1]);
//                face.normalVec = tempNormal.normalization();
//                cout<< face.normalVec.x << " " << face.normalVec.y << " " << face.normalVec.z << " " <<endl;
//            }
//        }

        int x=1;

        for (int y = 0; y < height; ++y) {
            cout << "Progress: " << x++ << "/"<< height<< endl;
            for (int x = 0; x < width; ++x) {

                Ray myRay = getRay(camera, x, y);

                double tmin = DBL_MAX;
                int closestObjIndex = -1;

                for(int s=0; s< my_spheres.size() ; s++){

                    double t = intersectSphere(myRay, my_spheres[s], my_vertices);
                    if (t>1 && t< tmin){

                        materialid=my_spheres[s].material_id;
                        myMaterial=my_materials[materialid-1];
                        ambientReflectance=myMaterial.ambient;
                        diffuseReflectance=myMaterial.diffuse;
                        specularReflectance=myMaterial.specular;
                        phongExp = myMaterial.phong_exponent;

                        tmin = t;
                        whichObj=1;
                        closestObjIndex = s;
                    }
                }

                for(int s=0; s< my_triangles.size() ; s++){

                    double t = intersectTriangle(myRay, my_triangles[s], my_vertices);
                    if (t>1 && t< tmin){
                        materialid=my_triangles[s].material_id;
                        myMaterial=my_materials[materialid-1];
                        ambientReflectance=myMaterial.ambient;
                        diffuseReflectance=myMaterial.diffuse;
                        specularReflectance=myMaterial.specular;
                        phongExp = myMaterial.phong_exponent;

                        tmin = t;
                        whichObj=2;
                        closestObjIndex = s;

                        kesisenTriangle=my_triangles[s];
                    }
                }

                for(int s=0; s< my_meshes.size() ; s++){

                    for(Face face : my_meshes[s].faces){

                        myTriangle.indices = face;

                        double t = intersectTriangle(myRay, myTriangle, my_vertices);
                        if (t>1 && t< tmin){
                            materialid=my_meshes[s].material_id;
                            myMaterial=my_materials[materialid-1];
                            ambientReflectance=myMaterial.ambient;
                            diffuseReflectance=myMaterial.diffuse;
                            specularReflectance=myMaterial.specular;
                            phongExp = myMaterial.phong_exponent;

                            tmin = t;
                            whichObj=3;
                            closestObjIndex = s;

                            kesisenTriangle=myTriangle;
                        }
                    }
                }

                if(closestObjIndex != -1) {

                    myColor.r = ambientReflectance.x * ambientLight.x;
                    myColor.g = ambientReflectance.y * ambientLight.y;
                    myColor.b = ambientReflectance.z * ambientLight.z;

                    if (whichObj == 1) {
                        intersectionPoint = myRay.d * tmin + myRay.o;
                        normal = (intersectionPoint - my_vertices[my_spheres[closestObjIndex].center_vertex_id - 1]) /
                                 my_spheres[closestObjIndex].radius;
                    }
                    else if (whichObj == 2) {
                        intersectionPoint = myRay.d * tmin + myRay.o;
                        normal = (my_vertices[kesisenTriangle.indices.v2_id - 1] - my_vertices[kesisenTriangle.indices.v1_id - 1]).cross(my_vertices[kesisenTriangle.indices.v0_id - 1] -
                                my_vertices[kesisenTriangle.indices.v1_id - 1]);
                        normal = normal.normalization();
                    }
                    else if (whichObj == 3){
                        intersectionPoint = myRay.d * tmin + myRay.o;
                        normal = (my_vertices[kesisenTriangle.indices.v2_id - 1] - my_vertices[kesisenTriangle.indices.v1_id - 1]).cross(my_vertices[kesisenTriangle.indices.v0_id - 1] -
                                my_vertices[kesisenTriangle.indices.v1_id - 1]);
                        normal = normal.normalization();
                    }
                    for (int p = 0; p < my_pointlights.size(); p++) {

                        notNormalLightVector = (my_pointlights[p].position - intersectionPoint);
                        notNormalLightVectorLength = notNormalLightVector.length();
                        normalLightVector = notNormalLightVector.normalization();

                        bool inShadow = false;
                        double objectT;
//                        Ray s(intersectionPoint + normal*shadowRayEpsilon , notNormalLightVector);
                        Ray s(intersectionPoint, notNormalLightVector);

                        for(Sphere sphere : my_spheres){
                            objectT = intersectSphere(s, sphere, my_vertices);
                            if(objectT>shadowRayEpsilon && objectT < 1){
                                inShadow = true;
                                break;
                            };
                        }
                        if(inShadow == false){
                            for(Triangle triangle : my_triangles){
                                objectT = intersectTriangle(s, triangle, my_vertices);
                                if(objectT>shadowRayEpsilon && objectT < 1){
                                    inShadow = true;
                                    break;
                                };
                            }
                        }
                        if(inShadow == false){
                            for(int m=0; m< my_meshes.size() ; m++){
                                for(Face face : my_meshes[m].faces){
                                    myTriangle.indices = face;
                                    objectT = intersectTriangle(s, myTriangle, my_vertices);
                                    if(objectT>shadowRayEpsilon && objectT < 1){
                                        inShadow = true;
                                        break;
                                    };
                                }
                            }
                        }
                        if(inShadow){
                            continue;
                        }

                        katsayi = normal.dot(normalLightVector);
                        intensityForDistance = my_pointlights[p].intensity / (notNormalLightVectorLength * notNormalLightVectorLength);
                        if (katsayi > 0) {
                            myColor.r += (float) (diffuseReflectance.x * intensityForDistance.x * katsayi);
                            myColor.g += (float) diffuseReflectance.y * intensityForDistance.y * katsayi;
                            myColor.b += (float) diffuseReflectance.z * intensityForDistance.z * katsayi;
                        }
                        ViewVector = camera.position - intersectionPoint;
                        ViewVector = ViewVector.normalization();

                        halfVectorForSpecular = (ViewVector + normalLightVector).normalization();
                        katsayi = normal.dot(halfVectorForSpecular);
                        if(katsayi>0){
                            myColor.r += (float) (specularReflectance.x * intensityForDistance.x * pow(katsayi, phongExp));
                            myColor.g += (float) specularReflectance.y * intensityForDistance.y * pow(katsayi, phongExp);
                            myColor.b += (float) specularReflectance.z * intensityForDistance.z * pow(katsayi, phongExp);
                        }
                    }
                    r=(int )(myColor.r < 255 ? myColor.r :myColor.r= 255);
                    g=(int)(myColor.g < 255 ? myColor.g : myColor.g=255);
                    b=(int)(myColor.b < 255 ? myColor.b : myColor.b=255);
                    image[i++] =myColor.r-r<0.5? r:r+1;
                    image[i++] =myColor.g-g<0.5?g:g+1;
                    image[i++] =myColor.b-b<0.5?b:b+1;
                }
                else{
                    image[i++] = backColor.x;
                    image[i++] = backColor.y;
                    image[i++] = backColor.z;
                }
            }
        }

        write_ppm(camera.image_name.c_str(), image, width, height);
    }
}