#include <iostream>
#include "parser.h"
#include "ppm.h"

using namespace std;
using namespace parser;
typedef unsigned char RGB[3];

struct Ray
{
    Vec3f o, d;
    Ray(const Vec3f &o, const Vec3f &d) : o(o), d(d) {}
};

inline double dot(const Vec3f &a, const Vec3f &b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

bool intersect(const Ray &ray, double &t, Sphere sphere, vector<Vec3f> my_vertices)
{
    Vec3f centerOfSphere = my_vertices[sphere.center_vertex_id -1];
    float radiusOfSphere = sphere.radius;

    const Vec3f o = ray.o;
    const Vec3f d = ray.d;
    Vec3f oc;
    oc.x = o.x - centerOfSphere.x;
    oc.y = o.y - centerOfSphere.y;
    oc.z = o.z - centerOfSphere.z;

    const double b = 2*dot(d,oc);
    const double a = dot(d,d);
    const double c = dot(oc,oc)- radiusOfSphere*radiusOfSphere;

    double disc = b * b - 4 * a * c;
    if (disc < 0){
        return false;
    }
    disc = sqrt(disc);
    const double t0 = (-b - disc)/(2*a);
    const double t1 = (-b + disc)/(2*a);
    t = (t0 < t1) ? t0 : t1;
    return true;
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

int main(int argc, char* argv[])
{
	cout<< "maine girdik arklar"<< endl;

    // Sample usage for reading an XML scene file
    parser::Scene scene;

    scene.loadFromXml(argv[1]);

    int width = 640, height = 480;

    vector<Camera> my_cameras =  scene.cameras;
    vector<Sphere> my_spheres =  scene.spheres;
    vector<Vec3f> my_vertices = scene.vertex_data;
    vector<Triangle> my_triangles = scene.triangles;
    vector<Mesh> my_meshes = scene.meshes;
    vector<Material> my_materials = scene.materials;
    Vec3i backColor = scene.background_color;
    vector<PointLight> my_pointlights = scene.point_lights;
    Vec3f ambientLight = scene.ambient_light;

    Color pixel_col[height][width];

    Color white(255, 255, 255);

//    bool intersect(const Ray &ray, double &t, Sphere sphere, vector<Vec3f> my_vertices)

    for(parser::Camera camera : my_cameras ) {

        unsigned char *image = new unsigned char[width * height * 3];

        int i = 0;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {


                for(Sphere sphere : my_spheres){


                    Vec3f vector1;
                    vector1.x = x;
                    vector1.y = y;
                    vector1.z = 0;
                    Vec3f vector2;
                    vector2.x = 0;
                    vector2.y = 0;
                    vector2.z = 1;
                    const Ray ray(vector1, vector2);

                    double t;

                    if (intersect(ray, t, sphere, my_vertices))
                    {
                        pixel_col[y][x] = white;
                        cout<< "intersection successful"<< endl;
                        image[i++] = pixel_col[y][x].r;
                        image[i++] = pixel_col[y][x].g;
                        image[i++] = pixel_col[y][x].b;
                    }
                    else{
                        image[i++] = backColor.x;
                        image[i++] = backColor.y;
                        image[i++] = backColor.z;
                    }

                }
            }
        }

        write_ppm(camera.image_name.c_str(), image, width, height);
    }

}
