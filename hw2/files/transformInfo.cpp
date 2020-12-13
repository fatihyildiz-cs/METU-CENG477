//
// Created by Fatih Yildiz on 13.12.2020.
//

#include "transformInfo.h"

TransformInfo::TransformInfo(char type, int index) : type(type), index(index) {}

std::ostream &operator<<(std::ostream &os, const TransformInfo &info) {
    os << "type: " << info.type << " index: " << info.index;
    return os;
}
