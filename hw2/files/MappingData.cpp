//
// Created by Fatih Yildiz on 16.12.2020.
//

#include "MappingData.h"

MappingData::MappingData(float u, float v) : u(u), v(v) {}

std::ostream &operator<<(std::ostream &os, const MappingData &data) {
    os << "u: " << data.u << " v: " << data.v;
    return os;
}

MappingData::MappingData() {}
