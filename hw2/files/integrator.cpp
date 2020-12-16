#include <iostream>
#include "integrator.h"
#include "image.h"
#include "ctpl_stl.h"
#include "matrix.h"
#include "vector3f.h"
#include "jpeg.h"

using namespace std;
using namespace fst::math;
namespace fst
{
    MappingData Integrator::MapSphere(math::Vector3f &a,math::Vector3f &center,float &radius,int &height,int& width) const {


    float pi=3.141592;

        a=a-center;

        float Q = acos(a.y / radius);
        float  Y= atan2(a.z ,a.x);
        float u = (-Y + pi) / (2*pi);
        float v= Q/ pi;

        return MappingData(u,v);
    }
    Integrator::Integrator(const parser::Scene& parser)
    {
        m_scene.loadFromParser(parser);
    }

    Vector3f getColor(float u, float v, Texture texture){
        if (texture.m_appearance == "clamp") {
            u = fmax(0., fmin(1., u));
            v = fmax(0., fmin(1., v));
        } else {
            u -= floor(u);
            v -= floor(v);
        }
        u *= texture.m_width;
        if (u >= texture.m_width) u--;
        v *= texture.m_height;
        if (v >= texture.m_height) v--;
        Vector3f color;
        if (texture.m_interpolation == "bilinear") {

            const unsigned int p = u;
            const unsigned int q = v;
            const float dx = u - p;
            const float dy = v - q;
            const unsigned int pos = q * texture.m_width * 3 + p * 3;
            color.x = texture.m_image[pos] * (1 - dx) * (1 - dy);
            color.x += texture.m_image[pos + 3] * (dx) * (1 - dy);
            color.x += texture.m_image[pos + 3 + texture.m_width * 3] * (dx) * (dy);
            color.x += texture.m_image[pos + texture.m_width * 3] * (1 - dx) * (dy);

            color.y = texture.m_image[pos + 1] * (1 - dx) * (1 - dy);
            color.y += texture.m_image[pos + 3 + 1] * (dx) * (1 - dy);
            color.y += texture.m_image[pos + 3 + texture.m_width * 3 + 1] * (dx) * (dy);
            color.y += texture.m_image[pos + texture.m_width * 3 + 1] * (1 - dx) * (dy);

            color.z = texture.m_image[pos + 2] * (1 - dx) * (1 - dy);
            color.z += texture.m_image[pos + 3 + 2] * (dx) * (1 - dy);
            color.z += texture.m_image[pos + 3 + texture.m_width * 3 + 2] * (dx) * (dy);
            color.z += texture.m_image[pos + texture.m_width * 3 + 2] * (1 - dx) * (dy);
        }
        else {

            const unsigned int pos = ((int)v) * texture.m_width * 3 + ((int)u) * 3;
            color.x = texture.m_image[pos];
            color.y = texture.m_image[pos + 1];
            color.z = texture.m_image[pos + 2];
        }

        return color;
    }
    math::Vector3f Integrator::renderPixel(const Ray& ray, int depth) const
    {
        if (depth > m_scene.max_recursion_depth)
        {
            return math::Vector3f(0.0f, 0.0f, 0.0f);
        }

        bool isReplaceAll=0;
        HitRecord hit_record;
        hit_record.texture_id=-1;
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
                if(hit_record.texture_id!=-1 ){
                    Texture texture=m_scene.textures[hit_record.texture_id-1];
                    Vector3f tempdif;
                    tempdif=m_scene.materials[hit_record.material_id].m_diffuse;
                    if(hit_record.isSphere){


                        MappingData mappingData=MapSphere(intersection_point,hit_record.center,hit_record.radius,
                                                       texture.m_height,texture.m_width);

                        //this function can be used for mesh too.
                        tempdif = getColor(mappingData.u, mappingData.v, texture);

                        if(texture.m_decalMode=="replace_kd"){
                            tempdif=tempdif/255;
                        }
                        else if(texture.m_decalMode=="blend_kd"){
                            tempdif=(tempdif/255+m_scene.materials[hit_record.material_id-1].m_diffuse)/2;
                        }
                        else if(texture.m_decalMode=="replace_all"){

                            isReplaceAll=1;
                        }
                    }
                    else{
                        //implement mesh texture mapping here

                        //here we can use the getColor() function I used in sphere texture mapping
                    }
                        color = color +light.computeRadiance(light_pos_distance) * material.computeBrdf(to_light, -ray.get_direction(), hit_record.normal, tempdif,isReplaceAll)
                                +isReplaceAll*tempdif;
                }
                else{

                    color = color + light.computeRadiance(light_pos_distance) * material.computeBrdf(to_light, -ray.get_direction(), hit_record.normal);
                }
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

        for (auto& sphere : m_scene.spheres)
        {
            Matrix myMatrix;
            for(TransformInfo info : sphere.transformInfos){
                if(info.type == "t"){
                    Translation translation = m_scene.translations[info.index-1];
                    myMatrix.Translate(translation.x, translation.y, translation.z);

                }
                else if(info.type == "s"){
                    Scaling scaling = m_scene.scalings[info.index-1];
                    myMatrix.Scale(scaling.x, scaling.y, scaling.z);
                    sphere.m_radius*=scaling.x;

                }
                else if(info.type == "r"){
                    Rotation rotation = m_scene.rotations[info.index-1];
                    myMatrix.Rotate(rotation.x,rotation.y,rotation.z,rotation.angle);
                }
                else{}
            }
            sphere.m_center=myMatrix.doAllTransformations(sphere.m_center);
        }

        for (auto& mesh : m_scene.meshes)
        {
            Matrix myMatrix;
            for(TransformInfo info : mesh.transformInfos){

                if(info.type == "t"){
                    Translation translation = m_scene.translations[info.index-1];
                    myMatrix.Translate(translation.x, translation.y, translation.z);
                }
                else if(info.type == "s"){
                    Scaling scaling = m_scene.scalings[info.index-1];
                    myMatrix.Scale(scaling.x, scaling.y, scaling.z);
                }
                else if(info.type == "r"){
                    Rotation rotation = m_scene.rotations[info.index-1];
                    myMatrix.Rotate(rotation.x,rotation.y,rotation.z,rotation.angle);
                }
                else{}
            }
            for(int i=0;i<mesh.m_triangles.size();i++) {
                mesh.m_triangles[i].m_edge1.x+=mesh.m_triangles[i].m_v0.x;
                mesh.m_triangles[i].m_edge1.y+=mesh.m_triangles[i].m_v0.y;
                mesh.m_triangles[i].m_edge1.z+=mesh.m_triangles[i].m_v0.z;

                mesh.m_triangles[i].m_edge2.x+=mesh.m_triangles[i].m_v0.x;
                mesh.m_triangles[i].m_edge2.y+=mesh.m_triangles[i].m_v0.y;
                mesh.m_triangles[i].m_edge2.z+=mesh.m_triangles[i].m_v0.z;
                mesh.m_triangles[i].m_v0=myMatrix.doAllTransformations(mesh.m_triangles[i].m_v0);
                mesh.m_triangles[i].m_edge1=myMatrix.doAllTransformations(mesh.m_triangles[i].m_edge1);
                mesh.m_triangles[i].m_edge2=myMatrix.doAllTransformations(mesh.m_triangles[i].m_edge2);
                mesh.m_triangles[i].m_edge1.x-=mesh.m_triangles[i].m_v0.x;
                mesh.m_triangles[i].m_edge1.y-=mesh.m_triangles[i].m_v0.y;
                mesh.m_triangles[i].m_edge1.z-=mesh.m_triangles[i].m_v0.z;

                mesh.m_triangles[i].m_edge2.x-=mesh.m_triangles[i].m_v0.x;
                mesh.m_triangles[i].m_edge2.y-=mesh.m_triangles[i].m_v0.y;
                mesh.m_triangles[i].m_edge2.z-=mesh.m_triangles[i].m_v0.z;
                mesh.m_triangles[i].m_normal=math::normalize(math::cross(mesh.m_triangles[i].m_edge1,mesh.m_triangles[i].m_edge2));
            }
        }
    }
    void Integrator::doTextureMapping() {
        int i=1;
        for(Sphere sphere : m_scene.spheres){
            cout <<"Sphere number " << i++ << ": " << sphere.textureId << endl;
        }
    }

    void Integrator::integrate()
    {

        doTransformations();
//        doTextureMapping();
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
