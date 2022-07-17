#pragma once
using namespace std;

const float PI = 3.141592f;

float Radian2Degree(float radian) { return radian * 180.0f / PI; };
float Degree2Radian(float degree) { return degree * PI / 180.0f; };

struct Vector3
{
public:
	float x;
	float y;
	float z;

public:
	Vector3() : x(0), y(0), z(0) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	static Vector3 Zero() { return Vector3(); }
	static Vector3 One() { return Vector3(1.0f, 1.0f, 1.0f); }
	static Vector3 Up() { return Vector3(0.0f, 1.0f, 0.0f); }
	static Vector3 Right() { return Vector3(1.0f, 0.0f, 0.0f); }
	static Vector3 Forward() { return Vector3(0.0f, 0.0f, 1.0f); }
};

struct Vector4
{
public:
	float x;
	float y;
	float z;
	float w;// 0 : πÊ«‚, 1 : ¡°

public:
	Vector4() : x(0), y(0), z(0), w(0) {};
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vector4(Vector3 v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
	Vector3 XYZ() { return Vector3(x, y, z); }
	static Vector4 Zero() { return Vector4(); }
	static Vector4 One() { return Vector4(1.0f, 1.0f, 1.0f, 1.0f); }
	Vector4 operator+(Vector4 v) { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
	Vector4 operator+(float a) { return Vector4(x + a, y + a, z + a, w + a); }
	Vector4 operator-(Vector4 v) { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
	Vector4 operator-(float a) { return Vector4(x - a, y - a, z - a, w - a); }
	Vector4 operator-() { return Vector4(-x, -y, -z, -w); }
	Vector4 operator*(float a) { return Vector4(x * a, y * a, z * a, w * a); }
	Vector4 operator/(float a) { return Vector4(x / a, y / a, z / a, w / a); }
};

struct Matrix2x2
{
public:
	float m00, m01;
	float m10, m11;

public:
	Matrix2x2() :m00(0), m01(0), m10(0), m11(0) {}
	Matrix2x2(float m00, float m01, float m10, float m11) :m00(m00), m01(m01), m10(m10), m11(m11) {}
	float Determinant() { return m00 * m11 - m01 * m10; }
	Matrix2x2 Inverse() { float determinantInverse = 1.0f / Determinant(); return Matrix2x2(m11, -m01, -m10, m11) * determinantInverse; }
	Matrix2x2 operator*(float a) { return Matrix2x2(m00 * a, m01 * a, m10 * a, m11 * a); }
};

struct Matrix3x3
{
public:
	float m00, m01, m02;
	float m10, m11, m12;
	float m20, m21, m22;

public:
	Matrix3x3() :m00(0), m01(0), m02(0), m10(0), m11(0), m12(0), m20(0), m21(0), m22(0) {}
	Matrix3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) :m00(m00), m01(m01), m02(m02), m10(m10), m11(m11), m12(m12), m20(m20), m21(m21), m22(m22) {}
	float Determinant() { return(m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20 - m01 * m10 * m22 - m00 * m12 * m21); }
	Matrix3x3 Inverse() { float determinantInverse = 1.0f / Determinant(); return Matrix3x3(m11 * m22 - m12 * m21, m02 * m21 - m01 * m22, m01 * m12 - m02 * m11, m12 * m20 - m10 * m22, m00 * m22 - m02 * m20, m02 * m10 - m00 * m12, m10 * m21 - m11 * m20, m01 * m20 - m00 * m21, m00 * m11 - m01 * m10) * determinantInverse; }
	Matrix3x3 operator*(float a) { return Matrix3x3(m00 * a, m01 * a, m02 * a, m10 * a, m11 * a, m12 * a, m20 * a, m21 * a, m22 * a); }
};
// http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche0023.html
// https://matrixcalc.org/ko/#determinant(%7B%7B1,2,3%7D,%7B4,4,3%7D,%7B1,1,3%7D%7D)
struct Matrix4x4
{
public:
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;
public:
	Matrix4x4() :m00(0), m01(0), m02(0), m03(0), m10(0), m11(0), m12(0), m13(0), m20(0), m21(0), m22(0), m23(0), m30(0), m31(0), m32(0), m33(0) {}
	Matrix4x4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) :m00(m00), m01(m01), m02(m02), m03(m03), m10(m10), m11(m11), m12(m12), m13(m13), m20(m20), m21(m21), m22(m22), m23(m23), m30(m30), m31(m31), m32(m32), m33(m33) {}
	Matrix4x4(Vector3 v1, Vector3 v2, Vector3 v3) :m00(v1.x), m01(v2.x), m02(v3.x), m03(0), m10(v1.y), m11(v2.y), m12(v3.y), m13(0), m20(v1.z), m21(v2.z), m22(v3.z), m23(0), m30(0), m31(0), m32(0), m33(1) {}
	Matrix4x4(Vector4 v1, Vector4 v2, Vector4 v3) :m00(v1.x), m01(v2.x), m02(v3.x), m03(0), m10(v1.y), m11(v2.y), m12(v3.y), m13(0), m20(v1.z), m21(v2.z), m22(v3.z), m23(0), m30(v1.w), m31(v2.w), m32(v3.w), m33(1) {}
	Matrix4x4 Transpose() { Matrix4x4 m; m.m01 = m10; m.m02 = m20; m.m03 = m30; m.m10 = m01; m.m12 = m21; m.m13 = m31; m.m20 = m02; m.m21 = m12; m.m23 = m32; m.m30 = m03; m.m31 = m13; m.m32 = m23; return m; }
	float Determinant() { return(m00 * m11 * m22 * m33 + m00 * m12 * m23 * m31 + m00 * m13 * m21 * m32 + m01 * m10 * m23 * m32 + m01 * m12 * m20 * m33 + m01 * m13 * m22 * m30 + m02 * m10 * m21 * m33 + m02 * m11 * m23 * m30 + m02 * m13 * m20 * m31 + m03 * m10 * m22 * m31 + m03 * m11 * m20 * m32 + m03 * m12 * m21 * m30 - m00 * m11 * m23 * m32 - m00 * m12 * m21 * m33 - m00 * m13 * m22 * m31 - m01 * m10 * m22 * m33 - m01 * m12 * m23 * m30 - m01 * m13 * m20 * m32 - m02 * m10 * m23 * m31 - m02 * m11 * m20 * m33 - m02 * m13 * m21 * m30 - m03 * m10 * m21 * m32 - m03 * m11 * m22 * m30 - m03 * m12 * m20 * m31); }
	Matrix4x4 Inverse() { float determinantInverse = 1.0f / Determinant(); return Matrix4x4(m11 * m22 * m33 + m12 * m23 * m31 + m13 * m21 * m32 - m11 * m23 * m32 - m12 * m21 * m33 - m13 * m22 * m31, m01 * m23 * m32 + m02 * m21 * m33 + m03 * m22 * m31 - m01 * m22 * m33 - m02 * m23 * m31 - m03 * m21 * m32, m01 * m12 * m33 + m02 * m13 * m31 + m03 * m11 * m32 - m01 * m13 * m32 - m02 * m11 * m33 - m03 * m12 * m31, m01 * m13 * m22 + m02 * m11 * m23 + m03 * m12 * m21 - m01 * m12 * m23 - m02 * m13 * m21 - m03 * m11 * m22, m10 * m23 * m32 + m12 * m20 * m33 + m13 * m22 * m30 - m10 * m22 * m33 - m12 * m23 * m30 - m13 * m20 * m32, m00 * m22 * m33 + m02 * m23 * m30 + m03 * m20 * m32 - m00 * m23 * m32 - m02 * m20 * m33 - m03 * m22 * m30, m00 * m13 * m32 + m02 * m10 * m33 + m03 * m12 * m30 - m00 * m12 * m33 - m02 * m13 * m30 - m03 * m10 * m32, m00 * m12 * m23 + m02 * m13 * m20 + m03 * m10 * m22 - m00 * m13 * m22 - m02 * m10 * m23 - m03 * m12 * m20, m10 * m21 * m33 + m11 * m23 * m30 + m13 * m20 * m31 - m10 * m23 * m31 - m11 * m20 * m33 - m13 * m21 * m30, m00 * m23 * m31 + m01 * m20 * m33 + m03 * m21 * m30 - m00 * m21 * m33 - m01 * m23 * m30 - m03 * m20 * m31, m00 * m11 * m33 + m01 * m13 * m30 + m03 * m10 * m31 - m00 * m13 * m31 - m01 * m10 * m33 - m03 * m11 * m30, m00 * m13 * m21 + m01 * m10 * m23 + m03 * m11 * m20 - m00 * m11 * m23 - m01 * m13 * m20 - m03 * m10 * m21, m10 * m22 * m31 + m11 * m20 * m32 + m12 * m21 * m30 - m10 * m21 * m32 - m11 * m22 * m30 - m12 * m20 * m31, m00 * m21 * m32 + m01 * m22 * m30 + m02 * m20 * m31 - m00 * m22 * m31 - m01 * m20 * m32 - m02 * m21 * m30, m00 * m12 * m31 + m01 * m10 * m32 + m02 * m11 * m30 - m00 * m11 * m32 - m01 * m12 * m30 - m02 * m10 * m31, m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m00 * m12 * m21 - m01 * m10 * m22 - m02 * m11 * m20) * determinantInverse; }
	static Matrix4x4 Zero() { return Matrix4x4(); }
	static Matrix4x4 Identity() { Matrix4x4 m; m.m00 = 1; m.m11 = 1; m.m22 = 1; m.m33 = 1; }
	static Matrix4x4 Translate(Vector3 v) { Matrix4x4 m; m = Matrix4x4::Identity(); m.m03 = v.x; m.m13 = v.y; m.m23 = v.z; return m; }
	static Matrix4x4 Translate(Vector4 v) { Matrix4x4 m; m = Matrix4x4::Identity(); m.m03 = v.x; m.m13 = v.y; m.m23 = v.z; return m; }
	static Matrix4x4 Transpose(Matrix4x4 m) { return m.Transpose(); }
	Matrix4x4 operator+(Matrix4x4 m) { return Matrix4x4(m00 + m.m00, m01 + m.m01, m02 + m.m02, m03 + m.m03, m10 + m.m10, m11 + m.m11, m12 + m.m12, m13 + m.m13, m20 + m.m20, m21 + m.m21, m22 + m.m22, m23 + m.m23, m30 + m.m30, m31 + m.m31, m32 + m.m32, m33 + m.m33); }
	Matrix4x4 operator+(float a) { return Matrix4x4(m00 + a, m01 + a, m02 + a, m03 + a, m10 + a, m11 + a, m12 + a, m13 + a, m20 + a, m21 + a, m22 + a, m23 + a, m30 + a, m31 + a, m32 + a, m33 + a); }
	Matrix4x4 operator-(Matrix4x4 m) { return Matrix4x4(m00 - m.m00, m01 - m.m01, m02 - m.m02, m03 - m.m03, m10 - m.m10, m11 - m.m11, m12 - m.m12, m13 - m.m13, m20 - m.m20, m21 - m.m21, m22 - m.m22, m23 - m.m23, m30 - m.m30, m31 - m.m31, m32 - m.m32, m33 - m.m33); }
	Matrix4x4 operator-(float a) { return Matrix4x4(m00 - a, m01 - a, m02 - a, m03 - a, m10 - a, m11 - a, m12 - a, m13 - a, m20 - a, m21 - a, m22 - a, m23 - a, m30 - a, m31 - a, m32 - a, m33 - a); }
	Matrix4x4 operator*(Matrix4x4 m) { return Matrix4x4(m00 * m.m00 + m01 * m.m10 + m02 * m.m20 + m03 * m.m30, m00 * m.m01 + m01 * m.m11 + m02 * m.m21 + m03 * m.m31, m00 * m.m02 + m01 * m.m12 + m02 * m.m22 + m03 * m.m32, m03 * m.m03 + m01 * m.m13 + m02 * m.m23 + m03 * m.m33, m10 * m.m00 + m11 * m.m10 + m12 * m.m20 + m13 * m.m30, m10 * m.m01 + m11 * m.m11 + m12 * m.m21 + m13 * m.m31, m10 * m.m02 + m11 * m.m12 + m12 * m.m22 + m13 * m.m32, m13 * m.m03 + m11 * m.m13 + m12 * m.m23 + m13 * m.m33, m20 * m.m00 + m21 * m.m10 + m22 * m.m20 + m23 * m.m30, m20 * m.m01 + m21 * m.m11 + m22 * m.m21 + m23 * m.m31, m20 * m.m02 + m21 * m.m12 + m22 * m.m22 + m23 * m.m32, m23 * m.m03 + m21 * m.m13 + m22 * m.m23 + m23 * m.m33, m30 * m.m00 + m31 * m.m10 + m32 * m.m20 + m33 * m.m30, m30 * m.m01 + m31 * m.m11 + m32 * m.m21 + m33 * m.m31, m30 * m.m02 + m31 * m.m12 + m32 * m.m22 + m33 * m.m32, m33 * m.m03 + m31 * m.m13 + m32 * m.m23 + m33 * m.m33); }
	Vector4 operator*(Vector4 v) { return Vector4(m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w, m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w, m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w, m30 * v.x + m31 * v.y + m32 * v.z + m33 * v.w); }
	Matrix4x4 operator*(float a) { return Matrix4x4(m00 * a, m01 * a, m02 * a, m03 * a, m10 * a, m11 * a, m12 * a, m13 * a, m20 * a, m21 * a, m22 * a, m23 * a, m30 * a, m31 * a, m32 * a, m33 * a); }
};