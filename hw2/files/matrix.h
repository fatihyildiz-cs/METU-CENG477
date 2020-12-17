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
    Matrix();
    ~Matrix();
    float **baseMatrix;
    float **baseMatrixduz;
    float  identity[4][4]={{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

    void Translate(float tx,float ty,float tz);

    void Rotate (float x,float y,float z,float alpha);

    void Scale(float sx,float sy,float sz);


    void rightMultiplyMatrix( float m1[][4]) ;
    void rightMultiplyMatrix( float **m1) ;

    void printMatrix();
     Vector3f doAllTransformations(Vector3f  point) ;
    void findBase(float x,float y,float z) const;


};
#endif //FILES_MATRIX_H
