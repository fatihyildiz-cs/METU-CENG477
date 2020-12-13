#pragma once


#include <ostream>

namespace fst
{
    class Scaling
    {
    public:
        float x, y, z;
        Scaling(float x, float y, float z);

        friend std::ostream &operator<<(std::ostream &os, const Scaling &scaling);
    };


}
