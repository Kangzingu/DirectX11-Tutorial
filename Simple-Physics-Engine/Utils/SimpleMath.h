#pragma once
#include <cmath>
#include <DirectXMath.h>
#include <limits>

using namespace std;

const float PI = 3.141592f;

struct General
{
public:
	static float RadianToDegree(float radian) { return radian * 180.0f / PI; };
	static float DegreeToRadian(float degree) { return degree * PI / 180.0f; };
};
struct Vector3
{
public:
	float x;
	float y;
	float z;

public:
	Vector3() : x(0), y(0), z(0) {}
	Vector3(DirectX::XMVECTOR v) : x(DirectX::XMVectorGetX(v)), y(DirectX::XMVectorGetY(v)), z(DirectX::XMVectorGetZ(v)) {}
	Vector3(DirectX::XMFLOAT3 v) : x(v.x), y(v.y), z(v.z) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	static Vector3 Zero() { return Vector3(0.0f, 0.0f, 0.0); }
	static Vector3 One() { return Vector3(1.0f, 1.0f, 1.0f); }
	static Vector3 Up() { return Vector3(0.0f, 1.0f, 0.0f); }
	static Vector3 Right() { return Vector3(1.0f, 0.0f, 0.0f); }
	static Vector3 Forward() { return Vector3(0.0f, 0.0f, 1.0f); }
	static Vector3 Normalize(Vector3 v) { float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z); return Vector3(v.x / length, v.y / length, v.z / length); }
	static Vector3 Cross(Vector3 v1, Vector3 v2) { return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }
	static float Magnitude(Vector3 v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
	static float SquareMagnitude(Vector3 v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
	static float Dot(Vector3 v1, Vector3 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
	static void GetOrthonormalBasis(Vector3& x, Vector3* y, Vector3* z)
	{
		// x축으로 부터 직교기저를 생성함, 실질적으로 x가 기준이 되고 y는 계산에 필요하기 때문에 임시로 주어지는것이기 떄문에 결국 바뀜
		*z = Vector3::Cross(x, *y);

		if (Vector3::SquareMagnitude(*z) == 0.0)
		{
			// 두 축이 평행해서 직교기저 생성이 안되니 둘 중 하나인 y를 변경함
			Vector3 modY = *y;
			y->x = modY.y;
			y->z = modY.x;
			y->y = modY.z;
			*z = Vector3::Cross(x, *y);
		}
		*y = Vector3::Cross(*z, x);

		x = Vector3::Normalize(x);
		*y = Vector3::Normalize(*y);
		*z = Vector3::Normalize(*z);
	}
	Vector3 operator+(Vector3 v) { return Vector3(x + v.x, y + v.y, z + v.z); }
	Vector3& operator+=(Vector3 v) { x += v.x; y += v.y; z += v.z;  return *this; }
	Vector3 operator+(float a) { return Vector3(x + a, y + a, z + a); }
	Vector3& operator+=(float a) { x += a; y += a; z += a;  return *this; }
	Vector3 operator-(Vector3 v) { return Vector3(x - v.x, y - v.y, z - v.z); }
	Vector3& operator-=(Vector3 v) { x -= v.x; y -= v.y; z -= v.z;  return *this; }
	Vector3 operator-(float a) { return Vector3(x - a, y - a, z - a); }
	Vector3& operator-=(float a) { x -= a; y -= a; z -= a;  return *this; }
	Vector3 operator-() { return Vector3(-x, -y, -z); }
	Vector3 operator*(float a) { return Vector3(x * a, y * a, z * a); }
	Vector3& operator*=(float a) { x *= a; y *= a; z *= a;  return *this; }
	Vector3 operator/(float a) { return Vector3(x / a, y / a, z / a); }
	Vector3& operator/=(float a) { x /= a; y /= a; z /= a;  return *this; }
	bool operator==(Vector3 v) { if (x == v.x && y == v.y && z == v.z)return true; else return false; }
	bool operator!=(Vector3 v) { if (x != v.x || y != v.y || z != v.z)return true; else return false; }
	DirectX::XMVECTOR ToXMVECTOR() { DirectX::XMVECTOR v = DirectX::XMVectorSet(x, y, z, 1); return v; };
	DirectX::XMFLOAT3 ToXMFLOAT3() { DirectX::XMFLOAT3 v = DirectX::XMFLOAT3(x, y, z); return v; };
};
struct Vector4
{
public:
	float x;
	float y;
	float z;
	float w;// 0 : 방향, 1 : 점

public:
	Vector4() : x(0), y(0), z(0), w(1) {};
	Vector4(DirectX::XMVECTOR v) : x(DirectX::XMVectorGetX(v)), y(DirectX::XMVectorGetY(v)), z(DirectX::XMVectorGetZ(v)), w(DirectX::XMVectorGetW(v)) {}
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vector4(Vector3 v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
	Vector3 XYZ() { return Vector3(x, y, z); }
	static Vector4 Zero() { return Vector4(0.0f, 0.0f, 0.0f, 0.0f); }
	static Vector4 One() { return Vector4(1.0f, 1.0f, 1.0f, 1.0f); }
	static Vector4 Normalize(Vector4 v)
	{
		float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
		if (length == 0)
		{
			return Vector4();
		}
		else
		{
			return Vector4(v.x / length, v.y / length, v.z / length, v.w / length);
		}
	}
	static float Magnitude(Vector4 v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w); }
	static float SquareMagnitude(Vector4 v) { return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w; }
	static float Dot(Vector4 v1, Vector4 v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w; }
	static Vector4 Quaternion(Vector3 axis, float angle)
	{
		angle = General::DegreeToRadian(angle);
		return Vector4(axis.x * sin(angle / 2.0f), axis.y * sin(angle / 2.0f), axis.z * sin(angle / 2.0f), cos(angle / 2.0f));
	}
	static Vector4 CombineQuaternion(Vector4 q1, Vector4 q2)
	{
		return Vector4(q1.w * q2.x + q1.x * q2.w - q1.y * q2.z - q1.z * q2.y,
					   q1.w * q2.y - q1.x * q2.z + q1.y * q2.w - q1.z * q2.x,
					   q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
					   q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
	}
	static Vector4 CombineQuaternionBookVersion(Vector4 q1, Vector4 q2)
	{
		return Vector4(q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
					   q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
					   q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
					   q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
	}
	Vector4 operator+(Vector4 v) { return Vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
	Vector4& operator+=(Vector4 v) { x += v.x; y += v.y; z += v.z; w += v.w;  return *this; }
	Vector4 operator+(float a) { return Vector4(x + a, y + a, z + a, w + a); }
	Vector4& operator+=(float a) { x += a; y += a; z += a; w += a;  return *this; }
	Vector4 operator-(Vector4 v) { return Vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
	Vector4& operator-=(Vector4 v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w;  return *this; }
	Vector4 operator-(float a) { return Vector4(x - a, y - a, z - a, w - a); }
	Vector4& operator-=(float a) { x -= a; y -= a; z -= a; w -= a;  return *this; }
	Vector4 operator-() { return Vector4(-x, -y, -z, -w); }
	Vector4 operator*(float a) { return Vector4(x * a, y * a, z * a, w * a); }
	Vector4& operator*=(float a) { x *= a; y *= a; z *= a; w *= a;  return *this; }
	Vector4 operator/(float a) { return Vector4(x / a, y / a, z / a, w / a); }
	Vector4& operator/=(float a) { x /= a; y /= a; z /= a; w /= a;  return *this; }
	bool operator==(Vector4 v) { if (x == v.x && y == v.y && z == v.z && w == v.w)return true; else return false; }
	bool operator!=(Vector4 v) { if (x != v.x || y != v.y || z != v.z || w != v.w)return true; else return false; }
	DirectX::XMVECTOR ToXMVECTOR() { DirectX::XMVECTOR v = DirectX::XMVectorSet(x, y, z, w); return v; };
	DirectX::XMFLOAT4 ToXMFLOAT4() { DirectX::XMFLOAT4 v = DirectX::XMFLOAT4(x, y, z, w); return v; };
};
struct Quaternion
{
public:
	float r;
	float i;
	float j;
	float k;

public:
	Quaternion() : r(1), i(0), j(0), k(0) {}
	Quaternion(float r, float i, float j, float k) : r(r), i(i), j(j), k(k) {}
	static Quaternion Identity() { return Quaternion(1.0f, 0.0f, 0.0f, 0.0f); }
	static Quaternion Normalize(Quaternion q)
	{
		float length = sqrt(q.r * q.r + q.i * q.i + q.j * q.j + q.k * q.k);
		return Quaternion(q.r / length, q.i / length, q.j / length, q.k / length);
	}
	Quaternion operator*(Quaternion q)
	{
		return Quaternion(r * q.r - i * q.i -
						  j * q.j - k * q.k,
						  r * q.i + i * q.r +
						  j * q.k - k * q.j,
						  r * q.j + j * q.r +
						  k * q.i - i * q.k,
						  r * q.k + k * q.r +
						  i * q.j - j * q.i);
	}
	Quaternion& operator*=(Quaternion q)
	{
		Quaternion me = *this;
		r = me.r * q.r - me.i * q.i -
			me.j * q.j - me.k * q.k;
		i = me.r * q.i + me.i * q.r +
			me.j * q.k - me.k * q.j;
		j = me.r * q.j + me.j * q.r +
			me.k * q.i - me.i * q.k;
		k = me.r * q.k + me.k * q.r +
			me.i * q.j - me.j * q.i;
		return *this;
	}
	Quaternion operator+(Vector3 rotation)
	{
		Quaternion q(0, rotation.x, rotation.y, rotation.z);
		q *= *this;
		return Quaternion(q.r + q.r / 2.0f,
						  q.i + q.i / 2.0f,
						  q.j + q.j / 2.0f,
						  q.k + q.k / 2.0f);
	}
	Quaternion& operator+=(Vector3 rotation)
	{
		Quaternion q(0, rotation.x, rotation.y, rotation.z);
		q *= *this;
		r += q.r / 2.0f;
		i += q.i / 2.0f;
		j += q.j / 2.0f;
		k += q.k / 2.0f;
		return *this;
	}
};
struct LineSegment
{
public:
	Vector3 origin;
	Vector3 direction;
	float length;
public:
	LineSegment() : origin(Vector3::Zero()), direction(Vector3::Zero()), length(0) {}
	LineSegment(Vector3 origin, Vector3 direction, float length) : origin(origin), direction(direction), length(length) {}
};
struct Matrix2x2
{
public:
	float m00, m01;
	float m10, m11;

public:
	Matrix2x2() :m00(1), m01(0), m10(1), m11(0) {}
	Matrix2x2(float m00, float m01, float m10, float m11) :m00(m00), m01(m01), m10(m10), m11(m11) {}
	float Determinant() { return m00 * m11 - m01 * m10; }
	Matrix2x2 Inverse() { float determinantInverse = 1.0f / Determinant(); return Matrix2x2(m11, -m01, -m10, m11) * determinantInverse; }
	Matrix2x2 operator*(float a) { return Matrix2x2(m00 * a, m01 * a, m10 * a, m11 * a); }
	Matrix2x2& operator*=(float a) { m00 *= a; m01 *= a; m10 *= a; m11 *= a; return *this; }
};
struct Matrix3x3
{
public:
	float m00, m01, m02;
	float m10, m11, m12;
	float m20, m21, m22;

public:
	Matrix3x3() :m00(1), m01(0), m02(0), m10(0), m11(1), m12(0), m20(0), m21(0), m22(1) {}
	Matrix3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) :m00(m00), m01(m01), m02(m02), m10(m10), m11(m11), m12(m12), m20(m20), m21(m21), m22(m22) {}
	float Determinant() { return(m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20 - m01 * m10 * m22 - m00 * m12 * m21); }
	static Matrix3x3 Zero() { return Matrix3x3(0, 0, 0, 0, 0, 0, 0, 0, 0); }
	Matrix3x3 Inverse()
	{
		float determinant = Determinant();
		if (determinant == 0)
			return Matrix3x3::Zero();
		float determinantInverse = 1.0f / Determinant(); return Matrix3x3(m11 * m22 - m12 * m21, m02 * m21 - m01 * m22, m01 * m12 - m02 * m11, m12 * m20 - m10 * m22, m00 * m22 - m02 * m20, m02 * m10 - m00 * m12, m10 * m21 - m11 * m20, m01 * m20 - m00 * m21, m00 * m11 - m01 * m10) * determinantInverse;
	}
	static Matrix3x3 SkewSymmetricForCross(const Vector3 v)
	{
		Matrix3x3 m;
		m.m00 = m.m11 = m.m22 = 0;
		m.m01 = -v.z;
		m.m02 = v.y;
		m.m10 = v.z;
		m.m12 = -v.x;
		m.m20 = -v.y;
		m.m21 = v.x;
		return m;
	}
	Matrix3x3 operator+(Matrix3x3 m)
	{
		return Matrix3x3(m00 + m.m00,
						 m01 + m.m01,
						 m02 + m.m02,
						 m10 + m.m10,
						 m11 + m.m11,
						 m12 + m.m12,
						 m20 + m.m20,
						 m21 + m.m21,
						 m22 + m.m22);
	}
	Matrix3x3& operator+=(Matrix3x3 m)
	{
		m00 += m.m00;
		m01 += m.m01;
		m02 += m.m02;
		m10 += m.m10;
		m11 += m.m11;
		m12 += m.m12;
		m20 += m.m20;
		m21 += m.m21;
		m22 += m.m22;
		return*this;
	}
	Matrix3x3 operator*(float a) { return Matrix3x3(m00 * a, m01 * a, m02 * a, m10 * a, m11 * a, m12 * a, m20 * a, m21 * a, m22 * a); }
	Matrix3x3& operator*=(float a) { m00 *= a; m01 *= a; m02 *= a; m10 *= a; m11 *= a; m12 *= a; m20 *= a; m21 *= a; m22 *= a; return *this; }
	Matrix3x3 operator*(Matrix3x3 m)
	{
		return Matrix3x3(m00 * m.m00 + m01 * m.m10 + m02 * m.m20,
						 m00 * m.m01 + m01 * m.m11 + m02 * m.m21,
						 m00 * m.m02 + m01 * m.m12 + m02 * m.m22,
						 m10 * m.m00 + m11 * m.m10 + m12 * m.m20,
						 m10 * m.m01 + m11 * m.m11 + m12 * m.m21,
						 m10 * m.m02 + m11 * m.m12 + m12 * m.m22,
						 m20 * m.m00 + m21 * m.m10 + m22 * m.m20,
						 m20 * m.m01 + m21 * m.m11 + m22 * m.m21,
						 m20 * m.m02 + m21 * m.m12 + m22 * m.m22);
	}
	Matrix3x3& operator*=(Matrix3x3 m)
	{
		Matrix3x3 me = *this;

		m00 = me.m00 * m.m00 + me.m01 * m.m10 + me.m02 * m.m20;
		m01 = me.m00 * m.m01 + me.m01 * m.m11 + me.m02 * m.m21;
		m02 = me.m00 * m.m02 + me.m01 * m.m12 + me.m02 * m.m22;
		m10 = me.m10 * m.m00 + me.m11 * m.m10 + me.m12 * m.m20;
		m11 = me.m10 * m.m01 + me.m11 * m.m11 + me.m12 * m.m21;
		m12 = me.m10 * m.m02 + me.m11 * m.m12 + me.m12 * m.m22;
		m20 = me.m20 * m.m00 + me.m21 * m.m10 + me.m22 * m.m20;
		m21 = me.m20 * m.m01 + me.m21 * m.m11 + me.m22 * m.m21;
		m22 = me.m20 * m.m02 + me.m21 * m.m12 + me.m22 * m.m22;
		return *this;
	}
	//Matrix3x3 operator*(Matrix4x4 m) { return Matrix3x3(m00 * m.m00 + m01 * m.m10 + m02 * m.m20, m00 * m.m01 + m01 * m.m11 + m02 * m.m21, m00 * m.m02 + m01 * m.m12 + m02 * m.m22, m10 * m.m00 + m11 * m.m10 + m12 * m.m20, m10 * m.m01 + m11 * m.m11 + m12 * m.m21, m10 * m.m02 + m11 * m.m12 + m12 * m.m22, m20 * m.m00 + m21 * m.m10 + m22 * m.m20, m20 * m.m01 + m21 * m.m11 + m22 * m.m21, m20 * m.m02 + m21 * m.m12 + m22 * m.m22); }
	//Matrix3x3& operator*=(Matrix4x4 m) { m00 = m00 * m.m00 + m01 * m.m10 + m02 * m.m20; m01 = m00 * m.m01 + m01 * m.m11 + m02 * m.m21; m02 = m00 * m.m02 + m01 * m.m12 + m02 * m.m22; m10 = m10 * m.m00 + m11 * m.m10 + m12 * m.m20; m11 = m10 * m.m01 + m11 * m.m11 + m12 * m.m21; m12 = m10 * m.m02 + m11 * m.m12 + m12 * m.m22; m20 = m20 * m.m00 + m21 * m.m10 + m22 * m.m20; m21 = m20 * m.m01 + m21 * m.m11 + m22 * m.m21; m22 = m20 * m.m02 + m21 * m.m12 + m22 * m.m22; return *this; }
	//Matrix3x3 Quaternion(Vector4 quaternion) { return Matrix3x3(1 - (2 * pow(quaternion.z, 2) + 2 * pow(quaternion.w, 2)), 2 * quaternion.y * quaternion.z + 2 * quaternion.w * quaternion.x, 2 * quaternion.y * quaternion.w - 2 * quaternion.z * quaternion.x, 2 * quaternion.y * quaternion.z - 2 * quaternion.w * quaternion.x, 1 - (2 * pow(quaternion.y, 2) + 2 * pow(quaternion.w, 2)), 2 * quaternion.z * quaternion.w + 2 * quaternion.y + quaternion.x, 2 * quaternion.y * quaternion.w + 2 * quaternion.z * quaternion.x, 2 * quaternion.z * quaternion.w - 2 * quaternion.y * quaternion.x, 1 - (2 * pow(quaternion.y, 2) + 2 * pow(quaternion.z, 2))); }
};
struct Matrix4x4
{
public:
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;

public:
	Matrix4x4() :m00(1), m01(0), m02(0), m03(0), m10(0), m11(1), m12(0), m13(0), m20(0), m21(0), m22(1), m23(0), m30(0), m31(0), m32(0), m33(1) {}
	Matrix4x4(DirectX::XMMATRIX m) :m00(DirectX::XMVectorGetX(m.r[0])), m01(DirectX::XMVectorGetX(m.r[1])), m02(DirectX::XMVectorGetX(m.r[2])), m03(DirectX::XMVectorGetX(m.r[3])), m10(DirectX::XMVectorGetY(m.r[0])), m11(DirectX::XMVectorGetY(m.r[1])), m12(DirectX::XMVectorGetY(m.r[2])), m13(DirectX::XMVectorGetY(m.r[3])), m20(DirectX::XMVectorGetZ(m.r[0])), m21(DirectX::XMVectorGetZ(m.r[1])), m22(DirectX::XMVectorGetZ(m.r[2])), m23(DirectX::XMVectorGetZ(m.r[3])), m30(DirectX::XMVectorGetW(m.r[0])), m31(DirectX::XMVectorGetW(m.r[1])), m32(DirectX::XMVectorGetW(m.r[2])), m33(DirectX::XMVectorGetW(m.r[3])) {}
	Matrix4x4(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) :m00(m00), m01(m01), m02(m02), m03(m03), m10(m10), m11(m11), m12(m12), m13(m13), m20(m20), m21(m21), m22(m22), m23(m23), m30(m30), m31(m31), m32(m32), m33(m33) {}
	Matrix4x4(Vector3 v1, Vector3 v2, Vector3 v3) :m00(v1.x), m01(v2.x), m02(v3.x), m03(0), m10(v1.y), m11(v2.y), m12(v3.y), m13(0), m20(v1.z), m21(v2.z), m22(v3.z), m23(0), m30(0), m31(0), m32(0), m33(1) {}
	Matrix4x4(Vector4 v1, Vector4 v2, Vector4 v3) :m00(v1.x), m01(v2.x), m02(v3.x), m03(0), m10(v1.y), m11(v2.y), m12(v3.y), m13(0), m20(v1.z), m21(v2.z), m22(v3.z), m23(0), m30(v1.w), m31(v2.w), m32(v3.w), m33(1) {}
	Matrix4x4(Vector4 v1, Vector4 v2, Vector4 v3, Vector4 v4) :m00(v1.x), m01(v2.x), m02(v3.x), m03(v4.x), m10(v1.y), m11(v2.y), m12(v3.y), m13(v4.y), m20(v1.z), m21(v2.z), m22(v3.z), m23(v4.z), m30(v1.w), m31(v2.w), m32(v3.w), m33(v4.w) {}
	Matrix4x4 Transpose()
	{
		Matrix4x4 m; m.m00 = m00; m.m01 = m10; m.m02 = m20; m.m03 = m30; m.m10 = m01; m.m11 = m11; m.m12 = m21; m.m13 = m31; m.m20 = m02; m.m21 = m12; m.m22 = m22; m.m23 = m32; m.m30 = m03; m.m31 = m13; m.m32 = m23; m.m33 = m33; return m;
	}
	float Determinant()
	{
		return(m00 * m11 * m22 * m33 + m00 * m12 * m23 * m31 + m00 * m13 * m21 * m32 + m01 * m10 * m23 * m32 + m01 * m12 * m20 * m33 + m01 * m13 * m22 * m30 + m02 * m10 * m21 * m33 + m02 * m11 * m23 * m30 + m02 * m13 * m20 * m31 + m03 * m10 * m22 * m31 + m03 * m11 * m20 * m32 + m03 * m12 * m21 * m30 - m00 * m11 * m23 * m32 - m00 * m12 * m21 * m33 - m00 * m13 * m22 * m31 - m01 * m10 * m22 * m33 - m01 * m12 * m23 * m30 - m01 * m13 * m20 * m32 - m02 * m10 * m23 * m31 - m02 * m11 * m20 * m33 - m02 * m13 * m21 * m30 - m03 * m10 * m21 * m32 - m03 * m11 * m22 * m30 - m03 * m12 * m20 * m31);
	}
	Matrix4x4 Inverse()
	{
		float determinant = Determinant();
		if (determinant == 0)
			return Matrix4x4::Zero();
		float determinantInverse = 1.0f / Determinant();
		return Matrix4x4(m11 * m22 * m33 + m12 * m23 * m31 + m13 * m21 * m32 - m11 * m23 * m32 - m12 * m21 * m33 - m13 * m22 * m31, m01 * m23 * m32 + m02 * m21 * m33 + m03 * m22 * m31 - m01 * m22 * m33 - m02 * m23 * m31 - m03 * m21 * m32, m01 * m12 * m33 + m02 * m13 * m31 + m03 * m11 * m32 - m01 * m13 * m32 - m02 * m11 * m33 - m03 * m12 * m31, m01 * m13 * m22 + m02 * m11 * m23 + m03 * m12 * m21 - m01 * m12 * m23 - m02 * m13 * m21 - m03 * m11 * m22, m10 * m23 * m32 + m12 * m20 * m33 + m13 * m22 * m30 - m10 * m22 * m33 - m12 * m23 * m30 - m13 * m20 * m32, m00 * m22 * m33 + m02 * m23 * m30 + m03 * m20 * m32 - m00 * m23 * m32 - m02 * m20 * m33 - m03 * m22 * m30, m00 * m13 * m32 + m02 * m10 * m33 + m03 * m12 * m30 - m00 * m12 * m33 - m02 * m13 * m30 - m03 * m10 * m32, m00 * m12 * m23 + m02 * m13 * m20 + m03 * m10 * m22 - m00 * m13 * m22 - m02 * m10 * m23 - m03 * m12 * m20, m10 * m21 * m33 + m11 * m23 * m30 + m13 * m20 * m31 - m10 * m23 * m31 - m11 * m20 * m33 - m13 * m21 * m30, m00 * m23 * m31 + m01 * m20 * m33 + m03 * m21 * m30 - m00 * m21 * m33 - m01 * m23 * m30 - m03 * m20 * m31, m00 * m11 * m33 + m01 * m13 * m30 + m03 * m10 * m31 - m00 * m13 * m31 - m01 * m10 * m33 - m03 * m11 * m30, m00 * m13 * m21 + m01 * m10 * m23 + m03 * m11 * m20 - m00 * m11 * m23 - m01 * m13 * m20 - m03 * m10 * m21, m10 * m22 * m31 + m11 * m20 * m32 + m12 * m21 * m30 - m10 * m21 * m32 - m11 * m22 * m30 - m12 * m20 * m31, m00 * m21 * m32 + m01 * m22 * m30 + m02 * m20 * m31 - m00 * m22 * m31 - m01 * m20 * m32 - m02 * m21 * m30, m00 * m12 * m31 + m01 * m10 * m32 + m02 * m11 * m30 - m00 * m11 * m32 - m01 * m12 * m30 - m02 * m10 * m31, m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m00 * m12 * m21 - m01 * m10 * m22 - m02 * m11 * m20) * determinantInverse;
	}
	static Matrix4x4 Zero()
	{
		return Matrix4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	}
	static Matrix4x4 Identity()
	{
		Matrix4x4 m; m.m00 = 1.0f; m.m11 = 1.0f; m.m22 = 1.0f; m.m33 = 1.0f; return m;
	}
	static Matrix4x4 Translation(Vector3 v)
	{
		Matrix4x4 m; m = Matrix4x4::Identity(); m.m03 = v.x; m.m13 = v.y; m.m23 = v.z; return m;
	}
	static Matrix4x4 Translation(Vector4 v)
	{
		Matrix4x4 m; m = Matrix4x4::Identity(); m.m03 = v.x; m.m13 = v.y; m.m23 = v.z; return m;
	}
	static Matrix4x4 Transpose(Matrix4x4& m)
	{
		return m.Transpose();
	}
	static Matrix4x4 Projection(float fovDegree, float aspectRatio, float nearZ, float farZ)
	{
		Matrix4x4 m; m.m00 = 1.0f / (aspectRatio * tan(fovDegree / 2.0f)); m.m11 = 1.0f / tan(fovDegree / 2.0f); m.m22 = -(farZ + nearZ) / (farZ - nearZ); m.m23 = -(2.0f * farZ * nearZ) / (farZ - nearZ); m.m32 = -1.0f; m.m33 = 0.0f; return m;
	}
	static Matrix4x4 Rotation(Vector3 angle)
	{
		Matrix4x4 mX, mY, mZ; mX.m11 = cos(angle.x); mX.m12 = sin(angle.x); mX.m21 = -sin(angle.x); mX.m22 = cos(angle.x); mY.m00 = cos(angle.y); mY.m02 = -sin(angle.y); mY.m20 = sin(angle.y); mY.m22 = cos(angle.y); mZ.m00 = cos(angle.z); mZ.m01 = sin(angle.z); mZ.m10 = -sin(angle.z); mZ.m11 = cos(angle.z); return mZ * mY * mX;
	}
	static Matrix4x4 Scaling(Vector3 scale)
	{
		Matrix4x4 m; m.m00 = scale.x; m.m11 = scale.y; m.m22 = scale.z; return m;
	}
	static Matrix4x4 RotationQuaternion(Vector3 axis, float angle)
	{
		axis = Vector3::Normalize(axis); Vector4 quaternion(axis.x * sin(angle / 2.0f), axis.y * sin(angle / 2.0f), axis.z * sin(angle / 2.0f), cos(angle / 2.0f)); Matrix4x4 m; m.m00 = 1.0f - 2.0f * (quaternion.y * quaternion.y + quaternion.z * quaternion.z); m.m01 = 2.0f * (quaternion.x * quaternion.y + quaternion.z * quaternion.w); m.m02 = 2.0f * (quaternion.x * quaternion.z - quaternion.y * quaternion.w); m.m10 = 2.0f * (quaternion.x * quaternion.y - quaternion.z * quaternion.w); m.m11 = 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.z * quaternion.z); m.m12 = 2.0f * (quaternion.y * quaternion.z + quaternion.x * quaternion.w); m.m20 = 2.0f * (quaternion.x * quaternion.z + quaternion.y * quaternion.w); m.m21 = 2.0f * (quaternion.y * quaternion.z - quaternion.x * quaternion.w); m.m22 = 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y); return m;
	}
	static Matrix4x4 RotationQuaternion(Vector4 quaternion)
	{
		Matrix4x4 m;
		m.m00 = 1.0f - 2.0f * (quaternion.y * quaternion.y + quaternion.z * quaternion.z);
		m.m01 = 2.0f * (quaternion.x * quaternion.y - quaternion.z * quaternion.w);
		m.m02 = 2.0f * (quaternion.x * quaternion.z + quaternion.y * quaternion.w);
		m.m10 = 2.0f * (quaternion.x * quaternion.y + quaternion.z * quaternion.w);
		m.m11 = 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.z * quaternion.z);
		m.m12 = 2.0f * (quaternion.y * quaternion.z - quaternion.x * quaternion.w);
		m.m20 = 2.0f * (quaternion.x * quaternion.z - quaternion.y * quaternion.w);
		m.m21 = 2.0f * (quaternion.y * quaternion.z + quaternion.x * quaternion.w);
		m.m22 = 1.0f - 2.0f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);
		return m;
	}
	static Matrix4x4 SphereInertiaTensor(float mass, float radius) { Matrix4x4 m; m.m00 = 2.0f * mass * radius * radius / 5.0f; m.m11 = 2.0f * mass * radius * radius / 5.0f; m.m22 = 2.0f * mass * radius * radius / 5.0f; return m; }
	static Matrix4x4 EmptySphereInertiaTensor(float mass, float radius) { Matrix4x4 m; m.m00 = 2.0f * mass * radius * radius / 3.0f; m.m11 = 2.0f * mass * radius * radius / 3.0f; m.m22 = 2.0f * mass * radius * radius / 3.0f; return m; }
	static Matrix4x4 CubeInertiaTensor(float mass, Vector3 scale)
	{
		Matrix4x4 m;
		m.m00 = mass * (scale.y * scale.y + scale.z * scale.z) / 12.0f; 
		m.m11 = mass * (scale.x * scale.x + scale.z * scale.z) / 12.0f; 
		m.m22 = mass * (scale.x * scale.x + scale.y * scale.y) / 12.0f; return m;
	}
	static Matrix4x4 Quaternion(Quaternion q)
	{
		Matrix4x4 m;
		m.m00 = 1.0f - 2.0f * (q.j * q.j + q.k * q.k);
		m.m01 = 2.0f * (q.i * q.j - q.r * q.k);
		m.m02 = 2.0f * (q.i * q.k + q.r * q.j);

		m.m10 = 2.0f * (q.i * q.j + q.r * q.k);
		m.m11 = 1.0f - 2.0f * (q.i * q.i + q.k * q.k);
		m.m12 = 2.0f * (q.j * q.k - q.r * q.i);

		m.m20 = 2.0f * (q.i * q.k - q.r * q.j);
		m.m21 = 2.0f * (q.j * q.k + q.r * q.i);
		m.m22 = 1.0f - 2.0f * (q.i * q.i + q.j * q.j);
		return m;
	}
	static Matrix4x4 SkewSymmetricForCross(const Vector4 v)
	{
		Matrix4x4 m = Matrix4x4::Zero();
		m.m01 = -v.z;
		m.m02 = v.y;
		m.m10 = v.z;
		m.m12 = -v.x;
		m.m20 = -v.y;
		m.m21 = v.x;
		return m;
	}
	Matrix4x4 operator+(Matrix4x4 m) { return Matrix4x4(m00 + m.m00, m01 + m.m01, m02 + m.m02, m03 + m.m03, m10 + m.m10, m11 + m.m11, m12 + m.m12, m13 + m.m13, m20 + m.m20, m21 + m.m21, m22 + m.m22, m23 + m.m23, m30 + m.m30, m31 + m.m31, m32 + m.m32, m33 + m.m33); }
	Matrix4x4& operator+=(Matrix4x4 m) { m00 += m.m00; m01 += m.m01; m02 += m.m02; m03 += m.m03; m10 += m.m10; m11 += m.m11; m12 += m.m12; m13 += m.m13; m20 += m.m20; m21 += m.m21; m22 += m.m22; m23 += m.m23; m30 += m.m30; m31 += m.m31; m32 += m.m32; m33 += m.m33; return*this; }
	Matrix4x4 operator+(float a) { return Matrix4x4(m00 + a, m01 + a, m02 + a, m03 + a, m10 + a, m11 + a, m12 + a, m13 + a, m20 + a, m21 + a, m22 + a, m23 + a, m30 + a, m31 + a, m32 + a, m33 + a); }
	Matrix4x4& operator+=(float a) { m00 += a; m01 += a; m02 += a; m03 += a; m10 += a; m11 += a; m12 += a; m13 += a; m20 += a; m21 += a; m22 += a; m23 += a; m30 += a; m31 += a; m32 += a; m33 += a; return*this; }
	Matrix4x4 operator-(Matrix4x4 m) { return Matrix4x4(m00 - m.m00, m01 - m.m01, m02 - m.m02, m03 - m.m03, m10 - m.m10, m11 - m.m11, m12 - m.m12, m13 - m.m13, m20 - m.m20, m21 - m.m21, m22 - m.m22, m23 - m.m23, m30 - m.m30, m31 - m.m31, m32 - m.m32, m33 - m.m33); }
	Matrix4x4& operator-=(Matrix4x4 m) { m00 -= m.m00; m01 -= m.m01; m02 -= m.m02; m03 -= m.m03; m10 -= m.m10; m11 -= m.m11; m12 -= m.m12; m13 -= m.m13; m20 -= m.m20; m21 -= m.m21; m22 -= m.m22; m23 -= m.m23; m30 -= m.m30; m31 -= m.m31; m32 -= m.m32; m33 -= m.m33; return*this; }
	Matrix4x4 operator-(float a) { return Matrix4x4(m00 - a, m01 - a, m02 - a, m03 - a, m10 - a, m11 - a, m12 - a, m13 - a, m20 - a, m21 - a, m22 - a, m23 - a, m30 - a, m31 - a, m32 - a, m33 - a); }
	Matrix4x4& operator-=(float a) { m00 -= a; m01 -= a; m02 -= a; m03 -= a; m10 -= a; m11 -= a; m12 -= a; m13 -= a; m20 -= a; m21 -= a; m22 -= a; m23 -= a; m30 -= a; m31 -= a; m32 -= a; m33 -= a; return*this; }
	Matrix4x4 operator*(Matrix4x4 m)
	{
		return Matrix4x4(
			m00 * m.m00 + m01 * m.m10 + m02 * m.m20 + m03 * m.m30,
			m00 * m.m01 + m01 * m.m11 + m02 * m.m21 + m03 * m.m31,
			m00 * m.m02 + m01 * m.m12 + m02 * m.m22 + m03 * m.m32,
			m00 * m.m03 + m01 * m.m13 + m02 * m.m23 + m03 * m.m33,
			m10 * m.m00 + m11 * m.m10 + m12 * m.m20 + m13 * m.m30,
			m10 * m.m01 + m11 * m.m11 + m12 * m.m21 + m13 * m.m31,
			m10 * m.m02 + m11 * m.m12 + m12 * m.m22 + m13 * m.m32,
			m10 * m.m03 + m11 * m.m13 + m12 * m.m23 + m13 * m.m33,
			m20 * m.m00 + m21 * m.m10 + m22 * m.m20 + m23 * m.m30,
			m20 * m.m01 + m21 * m.m11 + m22 * m.m21 + m23 * m.m31,
			m20 * m.m02 + m21 * m.m12 + m22 * m.m22 + m23 * m.m32,
			m20 * m.m03 + m21 * m.m13 + m22 * m.m23 + m23 * m.m33,
			m30 * m.m00 + m31 * m.m10 + m32 * m.m20 + m33 * m.m30,
			m30 * m.m01 + m31 * m.m11 + m32 * m.m21 + m33 * m.m31,
			m30 * m.m02 + m31 * m.m12 + m32 * m.m22 + m33 * m.m32,
			m30 * m.m03 + m31 * m.m13 + m32 * m.m23 + m33 * m.m33);
	}
	Matrix4x4& operator*=(Matrix4x4 m)
	{
		Matrix4x4 me = *this;
		m00 = me.m00 * m.m00 + me.m01 * m.m10 + me.m02 * m.m20 + me.m03 * m.m30;
		m01 = me.m00 * m.m01 + me.m01 * m.m11 + me.m02 * m.m21 + me.m03 * m.m31;
		m02 = me.m00 * m.m02 + me.m01 * m.m12 + me.m02 * m.m22 + me.m03 * m.m32;
		m03 = me.m00 * m.m03 + me.m01 * m.m13 + me.m02 * m.m23 + me.m03 * m.m33;
		m10 = me.m10 * m.m00 + me.m11 * m.m10 + me.m12 * m.m20 + me.m13 * m.m30;
		m11 = me.m10 * m.m01 + me.m11 * m.m11 + me.m12 * m.m21 + me.m13 * m.m31;
		m12 = me.m10 * m.m02 + me.m11 * m.m12 + me.m12 * m.m22 + me.m13 * m.m32;
		m13 = me.m10 * m.m03 + me.m11 * m.m13 + me.m12 * m.m23 + me.m13 * m.m33;
		m20 = me.m20 * m.m00 + me.m21 * m.m10 + me.m22 * m.m20 + me.m23 * m.m30;
		m21 = me.m20 * m.m01 + me.m21 * m.m11 + me.m22 * m.m21 + me.m23 * m.m31;
		m22 = me.m20 * m.m02 + me.m21 * m.m12 + me.m22 * m.m22 + me.m23 * m.m32;
		m23 = me.m20 * m.m03 + me.m21 * m.m13 + me.m22 * m.m23 + me.m23 * m.m33;
		m30 = me.m30 * m.m00 + me.m31 * m.m10 + me.m32 * m.m20 + me.m33 * m.m30;
		m31 = me.m30 * m.m01 + me.m31 * m.m11 + me.m32 * m.m21 + me.m33 * m.m31;
		m32 = me.m30 * m.m02 + me.m31 * m.m12 + me.m32 * m.m22 + me.m33 * m.m32;
		m33 = me.m30 * m.m03 + me.m31 * m.m13 + me.m32 * m.m23 + me.m33 * m.m33;
		return *this;
	}
	Vector4 operator*(Vector4 v)
	{
		return Vector4(
			m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w,
			m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w, 
			m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w, 
			m30 * v.x + m31 * v.y + m32 * v.z + m33 * v.w);
	}
	Vector3 operator*(Vector3 v) { return Vector3(m00 * v.x + m01 * v.y + m02 * v.z + m03, m10 * v.x + m11 * v.y + m12 * v.z + m13, m20 * v.x + m21 * v.y + m22 * v.z + m23); }
	Matrix4x4 operator*(float a) { return Matrix4x4(m00 * a, m01 * a, m02 * a, m03 * a, m10 * a, m11 * a, m12 * a, m13 * a, m20 * a, m21 * a, m22 * a, m23 * a, m30 * a, m31 * a, m32 * a, m33 * a); }
	Matrix4x4& operator*=(float a) { m00 *= a; m01 *= a; m02 *= a; m03 *= a; m10 *= a; m11 *= a; m12 *= a; m13 *= a; m20 *= a; m21 *= a; m22 *= a; m23 *= a; m30 *= a; m31 *= a; m32 *= a; m33 *= a; return*this; }
	Matrix4x4 operator=(DirectX::XMMATRIX m) { return Matrix4x4(m); }
	bool operator==(Matrix4x4 m) { if (m00 == m.m00 && m01 == m.m01 && m02 == m.m02 && m03 == m.m03 && m10 == m.m10 && m11 == m.m11 && m12 == m.m12 && m13 == m.m13 && m20 == m.m20 && m21 == m.m21 && m22 == m.m22 && m23 == m.m23 && m30 == m.m30 && m31 == m.m31 && m32 == m.m32 && m33 == m.m33)return true; else return false; }
	bool operator!=(Matrix4x4 m) { if (m00 != m.m00 || m01 != m.m01 || m02 != m.m02 || m03 != m.m03 || m10 != m.m10 || m11 != m.m11 || m12 != m.m12 || m13 != m.m13 || m20 != m.m20 || m21 != m.m21 || m22 != m.m22 || m23 != m.m23 || m30 != m.m30 || m31 != m.m31 || m32 != m.m32 || m33 != m.m33)return true; else return false; }
	DirectX::XMMATRIX ToXMMATRIX() { DirectX::XMMATRIX m = DirectX::XMMatrixSet(m00, m10, m20, m30, m01, m11, m21, m31, m02, m12, m22, m32, m03, m13, m23, m33); return m; }
};