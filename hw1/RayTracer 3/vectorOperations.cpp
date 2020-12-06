#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <cmath>
using namespace std;
using namespace parser;


Vec3f Vec3f::operator+(const Vec3f &v) { return Vec3f{x + v.x, y + v.y, z + v.z}; }
Vec3f Vec3f::operator-(const Vec3f &v) { return Vec3f{x - v.x, y - v.y, z - v.z}; }
Vec3f Vec3f::operator*(double d) { return Vec3f{float(x * d), float(y * d), float(z * d)}; }
Vec3f Vec3f::operator/(double d) { return Vec3f{ float(x / d), float(y / d), float(z / d)}; }
Vec3f Vec3f::cross(const Vec3f &v) { return Vec3f{y * v.z - v.y * z, v.x * z - x * v.z, x * v.y - v.x * y}; }
float Vec3f::dot(const Vec3f &v) { return x * v.x + y * v.y + z * v.z; }
bool Vec3f::operator!=(Vec3f v) { return (x != v.x || y != v.y || z != v.z); }
bool Vec3f::operator==(Vec3f v) { return (x == v.x && y == v.y && z == v.z); }
double Vec3f::length(){return sqrt(x*x+y*y+z*z);}
Vec3f Vec3f::normalization(){ double l= length(); if(l>0){return Vec3f{float(x/l),float(y/l),float(z/l)};} }



//Vec3i Vec3i::operator+(const Vec3i &v) const { return Vec3i{x + v.x, y + v.y, z + v.z}; }
//Vec3i Vec3i::operator-(const Vec3i &v) const { return Vec3i{x - v.x, y - v.y, z - v.z}; }
//Vec3i Vec3i::operator*(double d) const { return Vec3i{x * d, y * d, z * d}; }
//Vec3i Vec3i::operator/(double d) const { return Vec3i{x / d, y / d, z / d}; }
//
//Vec4f Vec4f::operator+(const Vec4f &v) const { return Vec4f{x + v.x, y + v.y, z + v.z, w + v.w}; }
//Vec4f Vec4f::operator-(const Vec4f &v) const { return Vec4f{x - v.x, y - v.y, z - v.z, w - v.w}; }
//Vec4f Vec4f::operator*(double d) const { return Vec4f{x * d, y * d, z * d, w * d}; }
//Vec4f Vec4f::operator/(double d) const { return Vec4f{x / d, y / d, z / d, w / d}; }