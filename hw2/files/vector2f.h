#pragma once

#include <ostream>

namespace fst
{
    namespace math
    {
        struct Vector2f
        {
            float x, y;

            Vector2f() = default;
            explicit Vector2f(float a)
                : x(a)
                , y(a)
            {}
            Vector2f(float a, float b)
                : x(a)
                , y(b)
            {}

            friend std::ostream &operator<<(std::ostream &os, const Vector2f &f) {
                os << "x: " << f.x << " y: " << f.y;
                return os;
            }
        };
    }
}