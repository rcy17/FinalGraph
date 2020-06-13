/*
* This file is merged from MIT Open Course 6-837 assignment Ray Casting
*/

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// DONE: implement this class and add more fields as necessary,
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
							hasTex(false)
	{
	}

	bool intersect(const Ray &ray, Hit &hit, double t_min) override
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
		auto normal = normals[0] * (1 - beta - gama) + normals[1] * beta + normals[2] * gama;
		normal.normalize();
		//hit.set(t, material, Vector3f::dot(d, normal) < 0 ? normal : -normal);
		hit.set(t, material, normal);
		if (hasTex)
		{
			hit.texCoord = texCoords[0] * (1 - beta - gama) + texCoords[1] * beta + texCoords[2] * gama;
			hit.hasTex = true;
		}
		return true;
	}
	bool hasTex;
	Vector3f normals[3];
	Vector2f texCoords[3];

protected:
	Vector3f vertices[3];
};

#endif //TRIANGLE_H
