#include "translation.h"

namespace fst
{
    Translation::Translation(float x, float y, float z)
        : x(x), y(y), z(z)
    {
    }

    std::ostream &operator<<(std::ostream &os, const Translation &translation) {
        os << "x: " << translation.x << " y: " << translation.y << " z: " << translation.z;
        return os;
    }
} // namespace fst
