#include <iostream>
#include "parser.h"
#include "ppm.h"

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


double intersectSphere(const Ray &ray, Sphere sphere, vector<Vec3f> my_vertices)
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
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {

                Ray myRay = getRay(camera, x, y);

                double tmin = 40000;
                int closestObjIndex = -1;

                for(int s=0; s< my_spheres.size() ; s++){

                    double t = intersectSphere(myRay, my_spheres[s], my_vertices);
                    if (t>1 && t< tmin){
                        tmin = t;
                        closestObjIndex = s;
                    }
                }
                if(closestObjIndex != -1){
                    image[i++] = 255;
                    image[i++] = 0;
                    image[i++] = 0;
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