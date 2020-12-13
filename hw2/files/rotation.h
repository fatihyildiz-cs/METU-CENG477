#pragma once

#include <ostream>

namespace fst
{
    class Rotation
    {
    public:
        float angle, x, y, z;
        Rotation(float angle, float x, float y, float z);

        friend std::ostream &operator<<(std::ostream &os, const Rotation &rotation);
    };


} // namespace fst
