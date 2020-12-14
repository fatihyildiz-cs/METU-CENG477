#include <sstream>
#include <iostream>
#include "scene.h"
#include "jpeg.h"
#include <iterator>
using namespace std;

namespace fst
{

    vector<TransformInfo> getTransformInfos(string transformString){

        string s = transformString;
        stringstream ss(s);
        istream_iterator<string> begin(ss);
        istream_iterator<string> end;
        vector<string> transformInfoStrings(begin, end);
        vector<TransformInfo> transformInfos;
        for(string s : transformInfoStrings){

            transformInfos.push_back(TransformInfo(s.substr(0,1), stoi(s.substr(1))));
        }
        return transformInfos;
    }

    void Scene::loadFromParser(const parser::Scene& parser)
    {
        for (auto& camera : parser.cameras)
        {
            cameras.push_back(Camera(
                math::Vector3f(camera.position.x, camera.position.y, camera.position.z),
                math::Vector3f(camera.gaze.x, camera.gaze.y, camera.gaze.z),
                math::Vector3f(camera.up.x, camera.up.y, camera.up.z),
                math::Vector4f(camera.near_plane.x, camera.near_plane.y, camera.near_plane.z, camera.near_plane.w),
                math::Vector2f(camera.image_width, camera.image_height),
                camera.image_name,
                camera.near_distance));
        }

        for (auto& pointlight : parser.point_lights)
        {
            point_lights.push_back(PointLight(
                math::Vector3f(pointlight.position.x, pointlight.position.y, pointlight.position.z),
                math::Vector3f(pointlight.intensity.x, pointlight.intensity.y, pointlight.intensity.z)));
        }

        for (auto& material : parser.materials)
        {
            materials.push_back(Material(
                math::Vector3f(material.ambient.x, material.ambient.y, material.ambient.z),
                math::Vector3f(material.diffuse.x, material.diffuse.y, material.diffuse.z),
                math::Vector3f(material.specular.x, material.specular.y, material.specular.z),
                math::Vector3f(material.mirror.x, material.mirror.y, material.mirror.z),
                material.phong_exponent));
        }

        for (auto &translation : parser.translations)
        {
            translations.push_back(
                Translation(translation.x, translation.y, translation.z));
        }

        for (auto &texture : parser.textures)
        {
            char* imageName = new char[texture.imageName.length()+1];
            int width;
            int height;

            for (int i = 0; i <= texture.imageName.length(); i++) {
                imageName[i] = texture.imageName[i];
            }
            read_jpeg_header(imageName, width, height);
            unsigned char* image = new unsigned char[width * height * 3];
            read_jpeg(imageName, image, width, height);
            textures.push_back(Texture(width, height,image, imageName, texture.interpolation, texture.decalMode, texture.appearance));
        }

        for (auto &scaling : parser.scalings)
        {
            scalings.push_back(
                Scaling(scaling.x, scaling.y, scaling.z));
        }

        for (auto &rotation : parser.rotations)
        {
            rotations.push_back(
                Rotation(rotation.angle, rotation.x, rotation.y, rotation.z));
        }

        for (auto& vertex : parser.vertex_data)
        {
            vertex_data.push_back(math::Vector3f(vertex.x, vertex.y, vertex.z));
        }

        for (auto& tex_coord : parser.tex_coord_data)
        {
            tex_coord_data.push_back(math::Vector2f(tex_coord.x, tex_coord.y));
        }

        for (auto& mesh : parser.meshes)
        {
            std::vector<Triangle> triangles;
            for (auto& face : mesh.faces)
            {
                triangles.push_back(Triangle(
                    vertex_data[face.v0_id - 1],
                    vertex_data[face.v1_id - 1] - vertex_data[face.v0_id - 1],
                    vertex_data[face.v2_id - 1] - vertex_data[face.v0_id - 1]));
            }
            vector<TransformInfo> transformInfos = getTransformInfos(mesh.transformations);

            int textureId = (mesh.texture_id > 0) && (mesh.texture_id <= parser.textures.size()) ? mesh.texture_id : -1;
            meshes.push_back(Mesh(std::move(triangles), mesh.material_id, transformInfos, textureId));
        }

        for (auto& triangle : parser.triangles)
        {
            std::vector<Triangle> triangles;

            triangles.push_back(Triangle(
                vertex_data[triangle.indices.v0_id - 1],
                vertex_data[triangle.indices.v1_id - 1] - vertex_data[triangle.indices.v0_id - 1],
                vertex_data[triangle.indices.v2_id - 1] - vertex_data[triangle.indices.v0_id - 1]));

            vector<TransformInfo> transformInfos = getTransformInfos(triangle.transformations);

            int textureId = (triangle.texture_id > 0) && (triangle.texture_id <= parser.textures.size()) ? triangle.texture_id : -1;
            meshes.push_back(Mesh(std::move(triangles), triangle.material_id, transformInfos, textureId));
        }

        for (auto& sphere : parser.spheres)
        {
            vector<TransformInfo> transformInfos = getTransformInfos(sphere.transformations);
            int textureId = (sphere.texture_id > 0) && (sphere.texture_id <= parser.textures.size()) ? sphere.texture_id : -1;
            spheres.push_back(Sphere(vertex_data[sphere.center_vertex_id - 1],
                sphere.radius, sphere.material_id, transformInfos, textureId));
        }

        background_color = math::Vector3f(parser.background_color.x, parser.background_color.y, parser.background_color.z);
        ambient_light = math::Vector3f(parser.ambient_light.x, parser.ambient_light.y, parser.ambient_light.z);
        shadow_ray_epsilon = parser.shadow_ray_epsilon;
        max_recursion_depth = parser.max_recursion_depth;
    }

    bool Scene::intersect(const Ray& ray, HitRecord& hit_record, float max_distance) const
    {
        HitRecord temp;
        float min_distance = max_distance;
        for (auto& sphere : spheres)
        {
            if (sphere.intersect(ray, temp, min_distance))
            {
                min_distance = temp.distance;
                hit_record = temp;
            }
        }

        for (auto& mesh : meshes)
        {
            if (mesh.intersect(ray, temp, min_distance))
            {
                min_distance = temp.distance;
                hit_record = temp;
            }
        }

        return min_distance != max_distance;
    }

    bool Scene::intersectShadowRay(const Ray& ray, float max_distance) const
    {
        for (auto& sphere : spheres)
        {
            if (sphere.intersectShadowRay(ray, max_distance))
            {
                return true;
            }
        }


        for (auto& mesh : meshes)
        {
            if (mesh.intersectShadowRay(ray, max_distance))
            {
		        return true;
            }
        }

	return false;
    }
}
