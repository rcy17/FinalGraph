#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle : public Object3D
{

public:
	Triangle() = delete;

	// a b c are three vertex positions of the triangle
	Triangle(const Vector3f &a,
			 const Vector3f &b,
			 const Vector3f &c,
			 Material *m) : Object3D(m),
							vertices{a, b, c},
							normal(Vector3f::cross(a - b, a - c))
	{
	}

	bool intersect(const Ray &ray, Hit &hit, float t_min) override
	{
		auto e1 = vertices[0] - vertices[1];
		auto e2 = vertices[0] - vertices[2];
		const auto &s = vertices[0] - ray.getOrigin();
		const auto &d = ray.getDirection();
		auto tem = Matrix3f(d, e1, e2).determinant();
		auto t = Matrix3f(s, e1, e2).determinant() / tem;
		if (t < t_min || t > hit.getT() || t <= 0)
			return false;
		auto beta = Matrix3f(d, s, e2).determinant() / tem;
		auto gama = Matrix3f(d, e1, s).determinant() / tem;
		if (beta < 0 || gama < 0 || beta + gama > 1)
			return false;
		hit.set(t, material, Vector3f::dot(d, normal) < 0 ? normal : -normal);
		return true;
	}
	Vector3f normal;
	Vector3f vertices[3];

protected:
};

#endif //TRIANGLE_H
