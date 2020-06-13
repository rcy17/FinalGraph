#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "texture.hpp"
#include "noise.hpp"
class Material
{
public:
    Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, double s = 0,
             double r = 0);

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const;

    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor);

    static Vector3f pointwiseDot(const Vector3f &v1, const Vector3f &v2);

    double clampedDot(const Vector3f &L, const Vector3f &N) const;
    void loadTexture(const char *filename);
    double getRefractionIndex();
    Vector3f getDiffuseColor();
    Vector3f getSpecularColor();

    void setNoise(const Noise &n);

protected:
    Vector3f diffuseColor;
    double refractionIndex;
    Vector3f specularColor;
    double shininess; // no use
    Texture t;
    Noise noise;
};

#endif // MATERIAL_H
