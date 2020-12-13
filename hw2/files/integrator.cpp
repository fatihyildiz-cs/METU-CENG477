#include <iostream>
#include "integrator.h"
#include "image.h"
#include "ctpl_stl.h"

using namespace std;

namespace fst
{
    Integrator::Integrator(const parser::Scene& parser)
    {
        m_scene.loadFromParser(parser);
    }

    math::Vector3f Integrator::renderPixel(const Ray& ray, int depth) const
    {
        if (depth > m_scene.max_recursion_depth)
        {
            return math::Vector3f(0.0f, 0.0f, 0.0f);
        }

        HitRecord hit_record;
        auto result = m_scene.intersect(ray, hit_record, std::numeric_limits<float>::max());

        if (!result)
        {
            return m_scene.background_color;
        }

        auto& material = m_scene.materials[hit_record.material_id - 1];
        auto color = material.get_ambient() * m_scene.ambient_light;
        auto intersection_point = ray.getPoint(hit_record.distance);

        for (auto& light : m_scene.point_lights)
        {
            auto to_light = light.get_position() - intersection_point;
            auto light_pos_distance = math::length(to_light);
            to_light = to_light / light_pos_distance;

            Ray shadow_ray(intersection_point + m_scene.shadow_ray_epsilon * to_light, to_light);

            if (!m_scene.intersectShadowRay(shadow_ray, light_pos_distance))
            {
                color = color + light.computeRadiance(light_pos_distance) * material.computeBrdf(to_light, -ray.get_direction(), hit_record.normal);
            }
        }

        auto& mirror = material.get_mirror();
        if (mirror.x + mirror.y + mirror.z > 0.0f)
        {
            auto new_direction = math::reflect(ray.get_direction(), hit_record.normal);
            Ray secondary_ray(intersection_point + m_scene.shadow_ray_epsilon * new_direction, new_direction);

            return color + mirror * renderPixel(secondary_ray, depth + 1);
        }
        else
        {
            return color;
        }
    }

    void Integrator::doTransformations() {

        int i=1;

        for (auto& sphere : m_scene.spheres)
        {
            cout << "Sphere " << i++ << endl;
            for(TransformInfo info : sphere.transformInfos){

                 if(info.type == "t"){
                    Translation translation = m_scene.translations[info.index-1];
                    cout << "Translation info: " << translation << endl;
                 }
                 else if(info.type == "s"){
                     Scaling scaling = m_scene.scalings[info.index-1];
                     cout << "Scaling info: " << scaling << endl;
                 }
                 else if(info.type == "r"){
                     Rotation rotation = m_scene.rotations[info.index-1];
                     cout << "Rotation info: " << rotation << endl;
                 }
                 else{}
            }
            cout << endl;
        }

        i = 1;
        for (auto& mesh : m_scene.meshes)
        {
            cout << "Mesh " << i++ << endl;
            for(TransformInfo info : mesh.transformInfos){

                if(info.type == "t"){
                    Translation translation = m_scene.translations[info.index-1];
                    cout << "Translation info: " << translation << endl;
                }
                else if(info.type == "s"){
                    Scaling scaling = m_scene.scalings[info.index-1];
                    cout << "Scaling info: " << scaling << endl;
                }
                else if(info.type == "r"){
                    Rotation rotation = m_scene.rotations[info.index-1];
                    cout << "Rotation info: " << rotation << endl;
                }
                else{}
            }
            cout << endl;
        }


// below lines print the individual TransformInfos for spheres and meshes. Note that triangle is treated as a mesh.
// We push the triangles into meshes array when loading from parser. Nothing special is necessary for triangles.

//        int i=1;
//        for (auto& sphere : m_scene.spheres)
//        {
//            cout << "Sphere number " << i++ << endl;
//
//            for(TransformInfo t : sphere.transformInfos){
//                cout << t << endl;
//            }
//            cout << endl;
//        }
//
//        i = 1;
//        for (auto& mesh : m_scene.meshes)
//        {
//            cout << "Mesh number " << i++ << endl;
//
//            for(TransformInfo t : mesh.transformInfos){
//                cout << t << endl;
//            }
//            cout << endl;
//        }

// below lines print the transformations that the scene has, not the individual objects. We reach these transformations
// by the indexes of object's TransformInfos array.

//        cout << "rotations: " << endl;
//        for( auto rotation : m_scene.rotations){
//            cout << rotation << endl;
//        }
//
//        cout << "scalings: " << endl;
//        for( auto scaling : m_scene.scalings){
//            cout << scaling << endl;
//        }
//
//        cout << "translations: " << endl;
//        for( auto translation : m_scene.translations){
//            cout << translation << endl;
//        }
    }

    void Integrator::integrate()
    {
        doTransformations();

        for (auto& camera : m_scene.cameras)
        {
            auto& resolution = camera.get_screen_resolution();
            Image image(resolution.x, resolution.y);

            ctpl::thread_pool pool(128);
            for (int i = 0; i < resolution.x; ++i)
            {
                pool.push([i, &resolution, &camera, &image, this](int id) {
                    for (int j = 0; j < resolution.y; ++j)
                    {
                        auto ray = camera.castPrimayRay(static_cast<float>(i), static_cast<float>(j));
                        auto color = renderPixel(ray, 0);
                        image.setPixel(i, j, math::clamp(color, 0.0f, 255.0f));
                    }
                });
            }
            pool.stop(true);

            image.save(camera.get_image_name());
        }
    }
}
