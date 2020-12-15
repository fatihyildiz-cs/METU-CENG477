#pragma once

#include "scene.h"

namespace fst
{
    class Integrator
    {
    public:
        Integrator(const parser::Scene& parser);

        math::Vector3f renderPixel(const Ray& ray, int depth) const ;
        void integrate();

        void doTransformations();
        int MapSphere(math::Vector3f a,math::Vector3f center,float radius,int height,int width)const;
        void doTextureMapping();
        Scene m_scene;
    private:



    };
}