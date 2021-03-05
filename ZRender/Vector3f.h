#ifndef _VECTOR3F_
#define _VECTOR3F_

#include <cmath>
#include <iostream>

class Vector3f
{
public:
	float x, y, z;
	Vector3f(float x = 0, float y = 0, float z = 0) :x(x), y(y), z(z) {}
	Vector3f operator*(const Vector3f& v) { return Vector3f(x * v.x, y * v.y, z * v.z); }
	//非成员函数操作符重载
	friend Vector3f operator*(const float f, const Vector3f& v) { return Vector3f(f * v.x, f * v.y, f * v.z);}
	friend Vector3f operator*(const Vector3f& v, const float f) { return Vector3f(f * v.x, f * v.y, f * v.z);}
	float norm(const Vector3f& v) const { return std::sqrt(x * x + y * y + z * z); }
	Vector3f normalize(const Vector3f& v) const 
	{
		if(norm(v) > 0) return Vector3f(x / norm(v), y / norm(v), z / norm(v)); 
		return v;
	}
	Vector3f operator+ (const Vector3f& v) const { return Vector3f(x + v.x, y + v.y, z + v.z); }
	Vector3f operator- (const Vector3f& v) const { return Vector3f(x - v.x, y - v.y, z - v.z); }
	Vector3f operator- () const { return Vector3f(-x, -y, -z); }

	friend std::ostream& operator<< (std::ostream& os, const Vector3f& v) { return os << v.x << std::endl << v.y << std::endl << v.z << std::endl; }

};
inline float dotProduct(const Vector3f& v1, const Vector3f& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
inline Vector3f crossProduct(const Vector3f& v1, const Vector3f& v2)
{
	return Vector3f(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

inline Vector3f lerp(const Vector3f& a, const Vector3f& b, const float& t)
{
	return  (1 - t) * a+ b * t;
}
#endif // _VECTOR_


