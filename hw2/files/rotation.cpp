#include "rotation.h"

namespace fst
{
    Rotation::Rotation(float angle, float x, float y, float z)
        : angle(angle)
        , x(x)
        , y(y)
        , z(z)
    {}

    std::ostream &operator<<(std::ostream &os, const Rotation &rotation) {
        os << "angle: " << rotation.angle << " x: " << rotation.x << " y: " << rotation.y << " z: " << rotation.z;
        return os;
    }
}
