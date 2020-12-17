#pragma once

#include "scene.h"
#include "vector3f.h"
#include "MappingData.h"

namespace fst
{
    class Integrator
    {
    public:
        Integrator(const parser::Scene& parser);

        math::Vector3f renderPixel(const Ray& ray, int depth) const ;
        void integrate();

        void doTransformations();
        MappingData MapSphere(math::Vector3f &a,math::Vector3f center,float &radius,int &height,int &width)const;
        MappingData MapTriangle(const HitRecord& hit_record, const Texture& texture, math::Vector3f barycentricCoordinates) const;

        math::Vector3f findBarycentricCoordinates(math::Vector3f P, Triangle triangle, math::Vector3f normal) const;
        void doTextureMapping();
        Scene m_scene;

        math::Vector3f convertFromGlobalToLocal(math::Vector3f point, math::Vector3f u, math::Vector3f v, math::Vector3f w) const;

    private:



    };
}