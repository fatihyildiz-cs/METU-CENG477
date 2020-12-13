#pragma once

#include <ostream>

namespace fst
{
    class Translation
    {
    public:
        float x, y, z;
        Translation(float x, float y, float z);

        friend std::ostream &operator<<(std::ostream &os, const Translation &translation);
    };


} // namespace fst
