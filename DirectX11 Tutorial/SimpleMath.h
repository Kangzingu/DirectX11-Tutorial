#pragma once
#include <cmath>

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
	static Vector3 operator+(Vector3 v1, Vector3 v2) { return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
	static Vector3 operator+(Vector3 v, float a) { return Vector3(v.x + a, v.y + a, v.z + a); }
	static Vector3 operator+(float a, Vector3 v) { return Vector3(a + v.x, a + v.y, a + v.z); }
	static Vector3 operator-(Vector3 v1, Vector3 v2) { return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
	static Vector3 operator-(Vector3 v, float a) { return Vector3(v.x - a, v.y - a, v.z - a); }
	static Vector3 operator-(float a, Vector3 v) { return Vector3(a - v.x, a - v.y, a - v.z); }
	static Vector3 operator-(Vector3 v) { return Vector3(-v.x, -v.y, -v.z); }
	static Vector3 operator*(Vector3 v, float a) { return Vector3(v.x * a, v.y * a, v.z * a); }
	static Vector3 operator*(float a, Vector3 v) { return Vector3(a * v.x, a * v.y, a * v.z); }
	static Vector3 operator/(Vector3 v, float b) { return Vector3(v.x / b, v.y / b, v.z / b); }
	static Vector3 operator/(float a, Vector3 v) { return Vector3(a / v.x, a / v.y, a / v.z); }
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
	static Vector4 operator+(Vector4 v1, Vector4 v2) { return Vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w); }
	static Vector4 operator+(Vector4 v, float a) { return Vector4(v.x + a, v.y + a, v.z + a, v.w + a); }
	static Vector4 operator+(float a, Vector4 v) { return Vector4(a + v.x, a + v.y, a + v.z, a + v.w); }
	static Vector4 operator-(Vector4 v1, Vector4 v2) { return Vector4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w); }
	static Vector4 operator-(Vector4 v, float a) { return Vector4(v.x - a, v.y - a, v.z - a, v.w - a); }
	static Vector4 operator-(float a, Vector4 v) { return Vector4(a - v.x, a - v.y, a - v.z, a - v.w); }
	static Vector4 operator-(Vector4 v) { return Vector4(-v.x, -v.y, -v.z, -v.w); }
	static Vector4 operator*(Vector4 v, float a) { return Vector4(v.x * a, v.y * a, v.z * a, v.w * a); }
	static Vector4 operator*(float a, Vector4 v) { return Vector4(a * v.x, a * v.y, a * v.z, a * v.w); }
	static Vector4 operator/(Vector4 v, float a) { return Vector4(v.x / a, v.y / a, v.z / a, v.w / a); }
	static Vector4 operator/(float a, Vector4 v) { return Vector4(a / v.x, a / v.y, a / v.z, a / v.w); }
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
	Matrix2x2 Inverse() { float determinantInverse = 1 / Determinant(); return Matrix2x2(determinantInverse * m11, determinantInverse * -m01, determinantInverse * -m10, determinantInverse * m00); }
};
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
	static Matrix4x4 Zero() { return Matrix4x4(); }
	static Matrix4x4 Identity() { Matrix4x4 m; m.m00 = 1; m.m11 = 1; m.m22 = 1; m.m33 = 1; }
	static Matrix4x4 Translate(Vector3 v) { Matrix4x4 m; m = Matrix4x4::Identity(); m.m03 = v.x; m.m13 = v.y; m.m23 = v.z; return m; }
	static Matrix4x4 Translate(Vector4 v) { Matrix4x4 m; m = Matrix4x4::Identity(); m.m03 = v.x; m.m13 = v.y; m.m23 = v.z; return m; }
	static Matrix4x4 Transpose(Matrix4x4 m) { return m.Transpose(); }
	static Matrix4x4 operator+(Matrix4x4 m1, Matrix4x4 m2) { return Matrix4x4(m1.m00 + m2.m00, m1.m01 + m2.m01, m1.m02 + m2.m02, m1.m03 + m2.m03, m1.m10 + m2.m10, m1.m11 + m2.m11, m1.m12 + m2.m12, m1.m13 + m2.m13, m1.m20 + m2.m20, m1.m21 + m2.m21, m1.m22 + m2.m22, m1.m23 + m2.m23, m1.m30 + m2.m30, m1.m31 + m2.m31, m1.m32 + m2.m32, m1.m33 + m2.m33); }
	static Matrix4x4 operator+(Matrix4x4 m, float a) { return Matrix4x4(m.m00 + a, m.m01 + a, m.m02 + a, m.m03 + a, m.m10 + a, m.m11 + a, m.m12 + a, m.m13 + a, m.m20 + a, m.m21 + a, m.m22 + a, m.m23 + a, m.m30 + a, m.m31 + a, m.m32 + a, m.m33 + a); }
	static Matrix4x4 operator+(float a, Matrix4x4 m) { return Matrix4x4(a + m.m00, a + m.m01, a + m.m02, a + m.m03, a + m.m10, a + m.m11, a + m.m12, a + m.m13, a + m.m20, a + m.m21, a + m.m22, a + m.m23, a + m.m30, a + m.m31, a + m.m32, a + m.m33); }
	static Matrix4x4 operator-(Matrix4x4 m1, Matrix4x4 m2) { return Matrix4x4(m1.m00 - m2.m00, m1.m01 - m2.m01, m1.m02 - m2.m02, m1.m03 - m2.m03, m1.m10 - m2.m10, m1.m11 - m2.m11, m1.m12 - m2.m12, m1.m13 - m2.m13, m1.m20 - m2.m20, m1.m21 - m2.m21, m1.m22 - m2.m22, m1.m23 - m2.m23, m1.m30 - m2.m30, m1.m31 - m2.m31, m1.m32 - m2.m32, m1.m33 - m2.m33); }
	static Matrix4x4 operator-(Matrix4x4 m, float a) { return Matrix4x4(m.m00 - a, m.m01 - a, m.m02 - a, m.m03 - a, m.m10 - a, m.m11 - a, m.m12 - a, m.m13 - a, m.m20 - a, m.m21 - a, m.m22 - a, m.m23 - a, m.m30 - a, m.m31 - a, m.m32 - a, m.m33 - a); }
	static Matrix4x4 operator-(float a, Matrix4x4 m) { return Matrix4x4(a - m.m00, a - m.m01, a - m.m02, a - m.m03, a - m.m10, a - m.m11, a - m.m12, a - m.m13, a - m.m20, a - m.m21, a - m.m22, a - m.m23, a - m.m30, a - m.m31, a - m.m32, a - m.m33); }
	static Matrix4x4 operator*(Matrix4x4 m1, Matrix4x4 m2) { return Matrix4x4(m1.m00 * m2.m00 + m1.m01 * m2.m10 + m1.m02 * m2.m20 + m1.m03 * m2.m30, m1.m00 * m2.m01 + m1.m01 * m2.m11 + m1.m02 * m2.m21 + m1.m03 * m2.m31, m1.m00 * m2.m02 + m1.m01 * m2.m12 + m1.m02 * m2.m22 + m1.m03 * m2.m32, m1.m03 * m2.m03 + m1.m01 * m2.m13 + m1.m02 * m2.m23 + m1.m03 * m2.m33, m1.m10 * m2.m00 + m1.m11 * m2.m10 + m1.m12 * m2.m20 + m1.m13 * m2.m30, m1.m10 * m2.m01 + m1.m11 * m2.m11 + m1.m12 * m2.m21 + m1.m13 * m2.m31, m1.m10 * m2.m02 + m1.m11 * m2.m12 + m1.m12 * m2.m22 + m1.m13 * m2.m32, m1.m13 * m2.m03 + m1.m11 * m2.m13 + m1.m12 * m2.m23 + m1.m13 * m2.m33, m1.m20 * m2.m00 + m1.m21 * m2.m10 + m1.m22 * m2.m20 + m1.m23 * m2.m30, m1.m20 * m2.m01 + m1.m21 * m2.m11 + m1.m22 * m2.m21 + m1.m23 * m2.m31, m1.m20 * m2.m02 + m1.m21 * m2.m12 + m1.m22 * m2.m22 + m1.m23 * m2.m32, m1.m23 * m2.m03 + m1.m21 * m2.m13 + m1.m22 * m2.m23 + m1.m23 * m2.m33, m1.m30 * m2.m00 + m1.m31 * m2.m10 + m1.m32 * m2.m20 + m1.m33 * m2.m30, m1.m30 * m2.m01 + m1.m31 * m2.m11 + m1.m32 * m2.m21 + m1.m33 * m2.m31, m1.m30 * m2.m02 + m1.m31 * m2.m12 + m1.m32 * m2.m22 + m1.m33 * m2.m32, m1.m33 * m2.m03 + m1.m31 * m2.m13 + m1.m32 * m2.m23 + m1.m33 * m2.m33); }
	static Vector4 operator*(Matrix4x4 m, Vector4 v) { return Vector4(m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w, m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w, m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w, m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w); }
	static Matrix4x4 operator*(Matrix4x4 m, float a) { return Matrix4x4(m.m00 * a, m.m01 * a, m.m02 * a, m.m03 * a, m.m10 * a, m.m11 * a, m.m12 * a, m.m13 * a, m.m20 * a, m.m21 * a, m.m22 * a, m.m23 * a, m.m30 * a, m.m31 * a, m.m32 * a, m.m33 * a); }
	static Matrix4x4 operator*(float a, Matrix4x4 m) { return Matrix4x4(a * m.m00, a * m.m01, a * m.m02, a * m.m03, a * m.m10, a * m.m11, a * m.m12, a * m.m13, a * m.m20, a * m.m21, a * m.m22, a * m.m23, a * m.m30, a * m.m31, a * m.m32, a * m.m33); }

};