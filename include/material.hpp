#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>

// DONE: Implement Shade function that computes Phong introduced in class.
class Material
{
public:
    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, double s = 0) : diffuseColor(d_color), specularColor(s_color), shininess(s)
    {
    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const
    {
        return diffuseColor;
    }

    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor)
    {
        Vector3f shaded = Vector3f::ZERO;
        const auto normal = hit.getNormal();
        shaded += diffuseColor * ReLU(Vector3f::dot(dirToLight, normal));
        const auto r_x = 2 * Vector3f::dot(dirToLight, normal) * normal - dirToLight;
        shaded += specularColor * pow(ReLU(Vector3f::dot(-ray.getDirection(), r_x)), shininess);
        shaded = lightColor * shaded;
        return shaded;
    }

protected:
    template <typename T>
    T ReLU(T x)
    {
        return x > 0 ? x : 0;
    }

    Vector3f diffuseColor;
    Vector3f specularColor;
    double shininess;
};

#endif // MATERIAL_H
