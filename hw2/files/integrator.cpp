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
    MappingData Integrator::MapSphere(math::Vector3f &a,math::Vector3f center,float &radius,int &height,int& width) const {


    float pi=3.141592;

         //a=a-center;

        float Q = acos(a.y / radius);
        float Y= atan2(a.z ,a.x);
        float u = (-Y + pi) / (2*pi);
        float v= Q/ pi;

        return MappingData(u,v);
    }


    MappingData Integrator::MapTriangle(const HitRecord& hit_record, const Texture& texture, math::Vector3f barycentricCoordinates) const {

        parser::Face face = hit_record.faceOfTheIntersectedTriangle;

        float beta = barycentricCoordinates.y;
        float gama = barycentricCoordinates.z;

        float ua = m_scene.tex_coord_data[face.v0_id-1].x;
        float va = m_scene.tex_coord_data[face.v0_id-1].y;

        float ub = m_scene.tex_coord_data[face.v1_id-1].x;
        float vb = m_scene.tex_coord_data[face.v1_id-1].y;

        float uc = m_scene.tex_coord_data[face.v2_id-1].x;
        float vc = m_scene.tex_coord_data[face.v2_id-1].y;

        float u = ua + beta*(ub-ua) + gama*(uc-ua);
        float v = va + beta*(vb-va) + gama*(vc-va);


        return MappingData(u,v);
    }

    math::Vector3f Integrator::findBarycentricCoordinates(math::Vector3f P, Triangle triangle, math::Vector3f normal) const {


        math::Vector3f a,b,c;

        a = triangle.m_v0;
        b = triangle.m_edge1 + triangle.m_v0;
        c = triangle.m_edge2 + triangle.m_v0;

        math::Vector3f bary ;

        // The area of a triangle is
        float areaABC = dot( normal, cross( (b - a), (c - a) )  ) ;
        float areaPBC = dot( normal, cross( (b - P), (c - P) )  ) ;
        float areaPCA = dot( normal, cross( (c - P), (a - P) )  ) ;

        bary.x = areaPBC / areaABC ; // alpha
        bary.y = areaPCA / areaABC ; // beta
        bary.z = 1.0f - bary.x - bary.y ; // gamma

        return bary ;
    }


    Integrator::Integrator(const parser::Scene& parser)
    {
        m_scene.loadFromParser(parser);
    }

    Vector3f getColor(float u, float v, const Texture& texture){
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

        bool isReplaceAll=false;
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
                    Vector3f newColor;
                    newColor=m_scene.materials[hit_record.material_id].m_diffuse;

                    MappingData mappingData;

                    if(hit_record.isSphere){

                        //implement sphere texture mapping here

                        math::Vector3f newIntersectionPoint=intersection_point;
                        newIntersectionPoint=newIntersectionPoint-hit_record.center;
                        newIntersectionPoint=convertFromGlobalToLocal(newIntersectionPoint, hit_record.intersectedSphere.m_u, hit_record.intersectedSphere.m_v, hit_record.intersectedSphere.m_w);

                        mappingData=MapSphere(newIntersectionPoint,hit_record.center,hit_record.radius,
                                                       texture.m_height,texture.m_width);
                    }
                    else{
                        //implement mesh texture mapping here

                        math::Vector3f barycentricCoordinates = findBarycentricCoordinates(intersection_point, hit_record.intersectedTriangle,hit_record.normal);
                        mappingData = MapTriangle(hit_record, texture, barycentricCoordinates);
                    }

                    newColor = getColor(mappingData.u, mappingData.v, texture);

                    if(texture.m_decalMode=="replace_kd"){
                        newColor= newColor / 255;
                    }
                    else if(texture.m_decalMode=="blend_kd"){
                        newColor= (newColor / 255 + m_scene.materials[hit_record.material_id - 1].m_diffuse) / 2;
                    }
                    else if(texture.m_decalMode=="replace_all"){

                        isReplaceAll=true;
                    }

                    color = color +light.computeRadiance(light_pos_distance) * material.computeBrdf(to_light, -ray.get_direction(), hit_record.normal, newColor, isReplaceAll)
                                + isReplaceAll * newColor;
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

    math::Vector3f Integrator::convertFromGlobalToLocal(math::Vector3f point, math::Vector3f u, math::Vector3f v, math::Vector3f w) const {

        Matrix Mconvert;
        float mat [4][4]={{u.x, u.y, u.z, 0},
                          {v.x, v.y, v.z,0},
                          {w.x, w.y, w.z, 0},
                          {0  , 0  , 0  , 1}};

        Mconvert.rightMultiplyMatrix(mat);

        point = Mconvert.doAllTransformations(point);

        return point;
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

            sphere.m_u = myMatrix.doAllTransformations(sphere.m_u);
            sphere.m_v = myMatrix.doAllTransformations(sphere.m_v);
            sphere.m_w = myMatrix.doAllTransformations(sphere.m_w);
            sphere.localCoordinateSystemCenter=myMatrix.doAllTransformations(sphere.localCoordinateSystemCenter);
            sphere.m_u=sphere.m_u-sphere.localCoordinateSystemCenter;
            sphere.m_v=sphere.m_v-sphere.localCoordinateSystemCenter;
            sphere.m_w=sphere.m_w-sphere.localCoordinateSystemCenter;
            sphere.m_w=normalize(sphere.m_w);
            sphere.m_u=normalize(sphere.m_u);
            sphere.m_v=normalize(sphere.m_v);

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
