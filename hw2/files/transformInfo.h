//
// Created by Fatih Yildiz on 13.12.2020.
//

#ifndef FILES_TRANSFORMINFO_H
#define FILES_TRANSFORMINFO_H


#include <ostream>

class TransformInfo {

public:
    std::string type;
    int index;

    TransformInfo(std::string type, int index);

    friend std::ostream &operator<<(std::ostream &os, const TransformInfo &info);
};


#endif //FILES_TRANSFORMINFO_H
