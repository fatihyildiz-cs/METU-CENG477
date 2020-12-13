#include "scaling.h"

namespace fst
{
    Scaling::Scaling(float x, float y, float z)
        : x(x)
        , y(y)
        , z(z)
    {}

    std::ostream &operator<<(std::ostream &os, const Scaling &scaling) {
        os << "x: " << scaling.x << " y: " << scaling.y << " z: " << scaling.z;
        return os;
    }
}
