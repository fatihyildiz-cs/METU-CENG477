#include <iostream>
#include "deneme.h"
#include "ppm.h"

using namespace std;
using namespace deneme;

Vec3f Vec3f::operator+(const Vec3f &v) const { return Vec3f{x + v.x, y + v.y, z + v.z}; }
Vec3f Vec3f::operator-(const Vec3f &v) const { return Vec3f{x - v.x, y - v.y, z - v.z}; }
Vec3f Vec3f::operator*(double d) const { return Vec3f{x * d, y * d, z * d}; }
Vec3f Vec3f::operator/(double d) const { return Vec3f{x / d, y / d, z / d}; }

Vec3i Vec3i::operator+(const Vec3i &v) const { return Vec3i{x + v.x, y + v.y, z + v.z}; }
Vec3i Vec3i::operator-(const Vec3i &v) const { return Vec3i{x - v.x, y - v.y, z - v.z}; }
Vec3i Vec3i::operator*(double d) const { return Vec3i{x * d, y * d, z * d}; }
Vec3i Vec3i::operator/(double d) const { return Vec3i{x / d, y / d, z / d}; }

Vec4f Vec4f::operator+(const Vec4f &v) const { return Vec4f{x + v.x, y + v.y, z + v.z, w+ v.w}; }
Vec4f Vec4f::operator-(const Vec4f &v) const { return Vec4f{x - v.x, y - v.y, z - v.z, w-v.w}; }
Vec4f Vec4f::operator*(double d) const { return Vec4f{x * d, y * d, z * d, w*d}; }
Vec4f Vec4f::operator/(double d) const { return Vec4f{x / d, y / d, z / d, w / d}; }

typedef unsigned char RGB[3];

int main(int argc, char* argv[]){
    Scene scene;

    scene.loadFromXml(argv[1]);
    const RGB BAR_COLOR[8] =
    {
        { 255, 255, 255 },  // 100% White
        { 255, 255,   0 },  // Yellow
        {   0, 255, 255 },  // Cyan
        {   0, 255,   0 },  // Green
        { 255,   0, 255 },  // Magenta
        { 255,   0,   0 },  // Red
        {   0,   0, 255 },  // Blue
        {   0,   0,   0 },  // Black
    };
    int width = 640, height = 480;
    int columnWidth = width / 8;
    unsigned char* image = new unsigned char [width * height * 3];
    int i = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int colIdx = x / columnWidth;
            image[i++] = BAR_COLOR[colIdx][0];
            image[i++] = BAR_COLOR[colIdx][1];
            image[i++] = BAR_COLOR[colIdx][2];
        }
    }

    write_ppm("test.ppm", image, width, height);

}