//
// Created by Fatih Yildiz on 16.12.2020.
//

#ifndef FILES_MAPPINGDATA_H
#define FILES_MAPPINGDATA_H


#include <ostream>

class MappingData {

public:
    float u;
    float v;

    MappingData();

    MappingData(float u, float v);

    friend std::ostream &operator<<(std::ostream &os, const MappingData &data);
};


#endif //FILES_MAPPINGDATA_H
