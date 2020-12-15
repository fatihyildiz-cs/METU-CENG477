//
// Created by simge on 13.12.2020.
//

#include "matrix.h"
#include "vector3f.h"


#define _USE_MATH_DEFINES

float pi = 3.14159265359;

using namespace std;
using namespace fst::math;

Matrix::Matrix() {
    baseMatrixduz=new float* [4];
    for(int i=0;i<4;i++)
        baseMatrixduz[i]=new float [4];
    baseMatrix=new float* [4];
    for(int i=0;i<4;i++)
        baseMatrix[i]=new float [4];

}
Matrix::~Matrix(){
    for(int i=0;i<4;i++){
        delete [] baseMatrixduz[i];
        delete [] baseMatrix[i];
    }
    baseMatrix=NULL;
    baseMatrixduz=NULL;

}
void Matrix::Translate(float tx,float ty,float tz){
    float translationMatrix[4][4]={{1,0,0,tx},{0,1,0,ty},{0,0,1,tz},{0,0,0,1}};

    rightMultiplyMatrix(translationMatrix);

}

void Matrix::Rotate (float x,float y,float z,float alpha){
    float rotationMatrix[4][4]={{1,0,0,0},{0,cos(alpha*pi/180), -sin(alpha*pi/180),0},{0,sin(alpha*pi/180),cos(alpha*pi/180),0},{0,0,0,1}};
    findBase(x,y,z);
    Translate(-x,-y,-z);
    rightMultiplyMatrix(baseMatrixduz);
    rightMultiplyMatrix(rotationMatrix);
    rightMultiplyMatrix(baseMatrix);
    Translate(x,y,z);
}
void Matrix::findBase(float x,float y,float z) {
    float boy=sqrt(x*x+y*y+z*z);
    x/=boy,y/=boy,z/=boy;
    float min=x<y?x:y;
    float a,b,c;
    min=min<z?min:z;
    if(min==x){
        a=0,b=-1*z,c=y;
    }
    else if(min==y) {
        b=0,c=x,a=-1*z;
    }
    else if (min==z){
        c=0,a=-1*y,b=x;
    }
    Vector3f result;
    result.x=y*c- 1*b * z;
    result.y= a * z -1* x * c;
    result.z=x * b - 1*a * y;

    boy=sqrt(result.x*result.x+result.y*result.y+result.z*result.z);
    result.x/=boy;
    result.y/=boy;
    result.z/=boy;
    boy=sqrt(a*a+b*b+c*c);
    a/=boy;
    b/=boy;
    c/=boy;

    baseMatrixduz[0][0]=x,baseMatrixduz[0][1]=y,baseMatrixduz[0][2]=z,baseMatrixduz[0][3]=0,
    baseMatrixduz[1][0]=a,baseMatrixduz[1][1]=b,baseMatrixduz[1][2]=c,baseMatrixduz[1][3]=0,
    baseMatrixduz[2][0]=result.x,baseMatrixduz[2][1]=result.y,baseMatrixduz[2][2]=result.z,baseMatrixduz[2][3]=0,
    baseMatrixduz[3][0]=0,baseMatrixduz[3][1]=0,baseMatrixduz[3][2]=0,baseMatrixduz[3][3]=1;

    baseMatrix[0][0]=x,baseMatrix[0][1]=a,baseMatrix[0][2]=result.x,baseMatrix[0][3]=0,
    baseMatrix[1][0]=y,baseMatrix[1][1]=b,baseMatrix[1][2]=result.y,baseMatrix[1][3]=0,
    baseMatrix[2][0]=z,baseMatrix[2][1]=c,baseMatrix[2][2]=result.z,baseMatrix[2][3]=0,
    baseMatrix[3][0]=0,baseMatrix[3][1]=0,baseMatrix[3][2]=0,baseMatrix[3][3]=1;
}


void  Matrix:: Scale(float sx,float sy,float sz) {
    float scaleMatrix[4][4]={{sx,0,0,0},{0,sy,0,0},{0,0,sz,0},{0,0,0,1}};
    rightMultiplyMatrix(scaleMatrix);
}


void Matrix:: rightMultiplyMatrix( float m1[4][4] ) {
    float total=0;
    float sonuc[4][4];

    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++) {
            total = 0;
            for (int k = 0; k < 4; k++) {
                total += m1[i][k] * identity[k][j];
            }
            sonuc[i][j] = total;

        }
    }

    for (int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            identity[i][j]=sonuc[i][j];


}
void Matrix:: rightMultiplyMatrix( float **m1 ) {
    float total=0;
    float sonuc[4][4];

    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++) {
            total = 0;
            for (int k = 0; k < 4; k++) {
                total += m1[i][k] * identity[k][j];
            }
            sonuc[i][j] = total;

        }
    }

    for (int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            identity[i][j]=sonuc[i][j];


}
  Vector3f Matrix:: doAllTransformations( fst::math::Vector3f   point ) {
    float oneDimensional[1][4];
    oneDimensional[0][0]=point.x;
    oneDimensional[0][1]=point.y;
    oneDimensional[0][2]=point.z;
    oneDimensional[0][3]=1;
    float total=0;
    float sonuc[1][4];

    for(int i=0;i<4;i++){
        total = 0;
        for(int j=0;j<4;j++) {
            total += identity[i][j] * oneDimensional[0][j];
        }
        sonuc[0][i] = total;
    }




   return Vector3f(sonuc[0][0],sonuc[0][1],sonuc[0][2]);
}
void Matrix:: printMatrix() {

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++) {
            cout << identity[i][j]<< " ";

        }
        cout << std::endl;
    }
}




