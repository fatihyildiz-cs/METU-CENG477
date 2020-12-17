#pragma once

#include <vector>
#include "vector3f.h"
#include "parser.h"
#include "triangle.h"
#include "sphere.h"

namespace fst
{
    struct HitRecord
    {
        math::Vector3f normal;
        float distance;
        int material_id;
        int texture_id;
        bool isSphere=0;
        math::Vector3f center;
        float radius;
        parser::Face faceOfTheIntersectedTriangle;
        Triangle intersectedTriangle;

        Sphere intersectedSphere;

        HitRecord() {}

    };
}