//
// Created by simge on 13.12.2020.
//

#include "matrix.h"
#include "vector3f.h"



using namespace std;


void Matrix::Translate(float tx,float ty,float tz){
    float translationMatrix[4][4]={{1,0,0,tx},{0,1,0,ty},{0,0,1,tz},{0,0,0,1}};
    rightMultiplyMatrix(translationMatrix);

}

void Matrix::Rotate (){
    float rotationMatrix[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
    rightMultiplyMatrix(rotationMatrix);
}

void  Matrix:: Scale(float sx,float sy,float sz) {
    float scaleMatrix[4][4]={{sx,0,0,0},{0,sy,0,0},{0,0,sz,0},{0,0,0,1}};
    rightMultiplyMatrix(scaleMatrix);
}


void Matrix:: rightMultiplyMatrix( float m1[][4] ) {
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
void Matrix:: doAllTransformations( fst::math::Vector3f  & point ) {
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
    point.x = sonuc[0][0];
    point.y = sonuc[0][1];
    point.z = sonuc[0][2];
}
void Matrix:: printMtrx() {

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++) {
            cout << identity[i][j]<< " ";

        }
        cout << std::endl;
    }
}




