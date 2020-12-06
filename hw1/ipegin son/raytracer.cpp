#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <cmath>

typedef unsigned char RGB[3];
using namespace std;
using namespace parser;

Ray getPrimaryRay( const Camera& camera, int row, int col)
{
    Vec3f u = camera.gaze.cross(camera.up);
    u = u.normalization();
    Vec3f up_vector = u.cross(camera.gaze);
    up_vector = up_vector.normalization();
    Vec3f gaze_vector = camera.gaze;
    gaze_vector = gaze_vector.normalization();
    float l = camera.near_plane.x;
    float r = camera.near_plane.y;
    float b = camera.near_plane.z;
    float t = camera.near_plane.w;
    float pixel_width = (r - l) / camera.image_width;
    float pixel_heigth = (t - b) / camera.image_height;
    Ray primaryRay;
    Vec3f su, sv, s;
    primaryRay.origin = camera.position;
    su = u * ((row + 0.5) * pixel_width);
    sv = up_vector * ((col + 0.5) * pixel_heigth);
    Vec3f q = camera.position+ gaze_vector*camera.near_distance + u*l+up_vector*t;

    s = su - sv+q;
    

    primaryRay.direction = s-camera.position;
    return primaryRay;
}

double determinant( const Vec3f& a, const Vec3f& b , const Vec3f& c)
{
    return (a.x*b.y*c.z + a.z*b.x*c.y + a.y*b.z*c.x) - (c.x*b.y*a.z+c.y*b.z*a.x+a.y*b.x*c.z); 
}

double intersect_triangle(const Ray &ray, Triangle& triangle, vector<Vec3f>& vertices,Vec3f &point,Vec3f &normal)
{
    Vec3f index1 = vertices[triangle.indices.v0_id-1];
    Vec3f index2 = vertices[triangle.indices.v1_id-1];
    Vec3f index3 = vertices[triangle.indices.v2_id-1];
    Vec3f normalHelper;
    double determinantA = determinant(index1-index2,index1-index3,ray.direction);
    double b = determinant(index1-ray.origin,index1-index3,ray.direction)/determinantA;
    double g = determinant(index1-index2,index1-ray.origin,ray.direction)/determinantA;
    double t = determinant(index1-index2,index1-index3,index1-ray.origin)/determinantA;
    if(b>=0 && g>=0 && t>=0 && b+g<=1)
    {
        normalHelper = (index3-index1).cross(index2-index1).normalization();
        if((ray.direction*-1).dot(normalHelper)>0)normal = normalHelper;
        else normal = normalHelper*-1;
        point = ray.origin + (ray.direction*t);
        return t;

    }
    else return -1;

}

double intersect_mesh(const Ray &ray, Mesh& mesh, vector<Vec3f>& vertices,Vec3f &point,Vec3f &normal)
{
    double t_temp = -1;
    double t = -1;
    for(int i=0;i<mesh.faces.size();i++)
    {
        Triangle triangle;
        triangle.material_id = mesh.material_id;
        triangle.indices = mesh.faces[i];
        t_temp = intersect_triangle(ray,triangle,vertices,point,normal);
        if(t_temp!=-1)
        {
            if(t!=-1)
            {
                if(t_temp<t)
                {
                    t = t_temp;
                }
            }
            else
            {
                t = t_temp;
            }
        }
    }
    return t;

}


double intersect(const Ray &ray, Sphere &sphere, vector<Vec3f> &vertices,Vec3f &point,Vec3f &normal)
{
    Vec3f sphere_center = vertices[sphere.center_vertex_id - 1];
    //cout<<"CENTER   "<<" X:"<<sphere_center.x<<" Y:"<<sphere_center.y<<" Z:"<<sphere_center.z<<endl;
    //cout<<"ORIGIN   "<<" X:"<<ray.origin.x<<" Y:"<<ray.origin.y<<" Z:"<<ray.origin.z<<endl;
    //cout<<"DIRECTION   "<<" X:"<<ray.direction.x<<" Y:"<<ray.direction.y<<" Z:"<<ray.direction.z<<endl;
    //cout<<"RADIUS     "<<sphere.radius<<endl;    
    float sphere_radius = sphere.radius;
    double A, B, C; //constants for the quadratic function
    double delta;
    double t, t1, t2;
    C = (ray.origin.x - sphere_center.x) * (ray.origin.x - sphere_center.x) + (ray.origin.y - sphere_center.y) * (ray.origin.y - sphere_center.y) +
        (ray.origin.z - sphere_center.z) * (ray.origin.z - sphere_center.z) - sphere_radius * sphere_radius;
    B =   ray.direction.x * (ray.origin.x - sphere_center.x) + ray.direction.y * (ray.origin.y - sphere_center.y) + ray.direction.z * (ray.origin.z - sphere_center.z);
    A = ray.direction.x * ray.direction.x + ray.direction.y * ray.direction.y + ray.direction.z * ray.direction.z;
    delta = B*B-A*C;
    if (delta < 0){
        return -1;
       
    }
        
    else if (delta == 0)
    {
        
        t = -B / A;
        if(t<0) t=-1;
        
    }
    else
    {
        double tmp;
        delta = sqrt(delta);
        t1 = (-B + delta) / A;
        t2 = (-B - delta) / A;

        if (t2 < t1)
        {
            tmp = t2;
            t2 = t1;
            t1 = tmp;
        }

        if(t1 > 0)
            t = t1;
        else if(t2 > 0)
            t=t2;
        else 
            t = -1;
    }
    point = ray.origin + (ray.direction*t);
    normal = (point-sphere_center).normalization();
    return t;
}


Vec3f colCalc(Ray &ray,vector<PointLight> &pointLights, Material &material,Vec3f &ambient_light,Vec3f &point,Vec3f &normal, vector<Mesh> &meshes,vector<Triangle> &triangles,vector<Sphere> &spheres,vector<Vec3f> &vertices)
{
    double cost;
    Vec3f ambient = {ambient_light.x*material.ambient.x,ambient_light.y*material.ambient.y,ambient_light.z*material.ambient.z};
    Vec3f returnVal = ambient;
    for(int k=0;k<pointLights.size();k++)
    {
        
        Vec3f wi = ( pointLights[k].position-point).normalization();
        if(wi.dot(normal.normalization()) > 0 ){
            cost = wi.dot(normal);
        }
        else cost=0;
        
        //cout << "distanceSq::::  "<< distanceSq<<endl;
        //cout << "cost::::  "<< cost<<endl;
        //cout<<"a:::: "<< a.x<<"   " << a.y << "    " << a.z<<endl;
        //cout<<"Material:::: "<< material.diffuse.x<<"   " << material.diffuse.y << "    " << material.diffuse.z<<endl;
        //cout<<"b:::: "<< b.x<<"   " << b.y << "    " << b.z<<endl;
        int flag = 1;
        Vec3f standart = {1,1,1};
        Vec3f pointToLightOrigin = (point+(wi*0.0001));
        Vec3f pointToLightDirection = wi;
        Ray pointToLight = {pointToLightDirection,pointToLightOrigin};
        Vec3f pointTri,normalTri;
        
        double lightT = (pointLights[k].position.x- pointToLightOrigin.x)/pointToLightDirection.x;
        for(int j=0;j<meshes.size();j++)
        {
            if(!flag) break;
            int t = intersect_mesh(pointToLight,meshes[j],vertices,pointTri,normalTri);
            if(t!=-1&&lightT>t )
            {
                flag = 0;
            }
        }
        for(int j=0;j<triangles.size();j++)
        {
            if(!flag) break;
            int t = intersect_triangle(pointToLight,triangles[j],vertices,pointTri,normalTri);
            if(t!=-1&&lightT>t )
            {
                flag = 0;
            }
        }
        for(int j=0;j<spheres.size();j++)
        {
            if(!flag) break;
            int t = intersect(pointToLight,spheres[j],vertices,pointTri,normalTri);
            if(t!=-1&&lightT>t)
            {
                flag = 0;
            }
        }
        if(flag)
        {
            double lightDistance = (pointLights[k].position-point).length();
            double distanceSq = lightDistance*lightDistance;
            Vec3f a = {pointLights[k].intensity.x/distanceSq,pointLights[k].intensity.y/distanceSq,pointLights[k].intensity.z/distanceSq};
            Vec3f b = a*cost;
            Vec3f diffuse = {b.x*material.diffuse.x,b.y*material.diffuse.y,b.z*material.diffuse.z};
            
            Vec3f halfVector = ((pointLights[k].position-point).normalization()+((ray.origin-point).normalization())).normalization();
            double cosa;
            if(halfVector.dot(normal.normalization())>0)cosa = halfVector.dot(normal.normalization());
            else cosa = 0;
            Vec3f c = a* pow(cosa,material.phong_exponent);
            Vec3f specular = {c.x*material.specular.x,c.y*material.specular.y,c.z*material.specular.z};
            if(diffuse.x+specular.x+returnVal.x>255) returnVal.x = 255;
            else returnVal.x = specular.x+diffuse.x+returnVal.x;
            if(diffuse.y+returnVal.y+specular.y>255) returnVal.y = 255;
            else returnVal.y = diffuse.y+returnVal.y+specular.y;
            if(diffuse.z+returnVal.z+specular.z>255) returnVal.z = 255;
            else returnVal.z = diffuse.z+returnVal.z+specular.z;
        }
         
        
    }
    
    return returnVal;
} 



int main(int argc, char *argv[])
{
    parser::Scene scene;
    scene.loadFromXml(argv[1]);
    vector<Camera> my_cameras = scene.cameras;
    Camera camera = my_cameras[0];
    int width = camera.image_width;
    int height = camera.image_height;
    int columnWidth = width / 8;
    unsigned char *image = new unsigned char[width * height * 3];
    vector<Sphere> spheres = scene.spheres;
    vector<Triangle> triangles = scene.triangles; 
    vector<Mesh> meshes = scene.meshes;
    vector<Vec3f> vertices = scene.vertex_data;
    Vec3f point,normal;
    Vec3f intersectPoint,intersectNormal;
    Color white{255, 255, 255};
    int material_id;
    int j = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Ray primaryRay = getPrimaryRay(camera, x, y);
            double tmin = 40000;
            int closestObj = -1;
            double t=-1;
            double t_temp=-1;
            for(int i=0;i<meshes.size();i++)
            {
                t_temp = intersect_mesh(primaryRay,meshes[i],vertices,point,normal);
                if(t_temp!=-1)
                {
                    if(t!=-1)
                    {
                        if(t_temp<t)
                        {
                            material_id = meshes[i].material_id;
                            intersectPoint = point;
                            intersectNormal = normal;
                            t = t_temp;
                        }
                    }
                    else
                    {
                        material_id = meshes[i].material_id;
                        intersectPoint = point;
                        intersectNormal = normal;
                        t = t_temp;
                    }
                }
            }
            for(int i=0;i<triangles.size();i++)
            {
                t_temp = intersect_triangle(primaryRay,triangles[i],vertices,point,normal);
                if(t_temp!=-1)
                {
                    if(t!=-1)
                    {
                        if(t_temp<t)
                        {
                            material_id = triangles[i].material_id;
                            intersectPoint = point;
                            intersectNormal = normal;
                            t = t_temp;
                        }
                    }
                    else
                    {
                        material_id = triangles[i].material_id;
                        intersectPoint = point;
                        intersectNormal = normal;
                        t = t_temp;
                    }
                }
            }
            
            for(int i=0;i<spheres.size();i++)
            {
                t_temp = intersect(primaryRay,spheres[i],vertices,point,normal);
                if(t_temp!=-1)
                {
                    if(t!=-1)
                    {
                        if(t_temp<t)
                        {
                            material_id = spheres[i].material_id;
                            intersectPoint = point;
                            intersectNormal = normal;
                            t = t_temp;
                        }
                    }
                    else
                    {
                        material_id = spheres[i].material_id;
                        intersectPoint = point;
                        intersectNormal = normal;
                        t = t_temp;
                    }
                }
            }
            
            if (t >= 1)
            {
                if (t < tmin)
                {
                    tmin = t;
                    closestObj = 0;
                }
            }
            Vec3f colorCal = {0,0,0};
            if (closestObj != -1)
            {
                colorCal = colCalc(primaryRay,scene.point_lights, scene.materials[material_id-1],scene.ambient_light,intersectPoint,intersectNormal,meshes,triangles,spheres,vertices);
            }
            
            image[j++] = colorCal.x;
            image[j++] = colorCal.y;
            image[j++] = colorCal.z;
        }
    }
    
    write_ppm("test.ppm", image, width, height);
}
