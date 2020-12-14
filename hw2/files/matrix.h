//
// Created by simge on 13.12.2020.
//
#include <cmath>
#include <iostream>
#include "vector3f.h"
#ifndef FILES_MATRIX_H
#define FILES_MATRIX_H

using namespace fst::math;
class Matrix {

public:


    float  identity[4][4]={{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

    void Translate(float tx,float ty,float tz);

    void Rotate ();

    void Scale(float sx,float sy,float sz);


    void rightMultiplyMatrix( float m1[][4]) ;

    void printMtrx();
    void doAllTransformations(Vector3f &point) ;



};
#endif //FILES_MATRIX_H
