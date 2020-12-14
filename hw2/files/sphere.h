#pragma once

#include <vector>
#include "vector3f.h"
#include "transformInfo.h"

namespace fst
{
    class Ray;
    struct HitRecord;

    class Sphere
    {
    public:
        Sphere(const math::Vector3f& center, float radius, int material_id);

        Sphere(const math::Vector3f& center, float radius, int material_id, std::vector<TransformInfo> transformInfos, int textureId);

        bool intersect(const Ray& ray, HitRecord& hit_record, float max_distance) const;
        bool intersectShadowRay(const Ray& ray, float max_distance) const;

        math::Vector3f m_center;

        std::vector<TransformInfo> transformInfos;

        float m_radius;

        int textureId;

    private:
        int m_material_id;
    };



}