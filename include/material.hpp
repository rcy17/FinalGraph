#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "texture.hpp"
#include "noise.hpp"
enum MeterialType
{
    ILLUMINANT,
    DIFFUSE,
    SPECULAR,
    REFRACTIVE,
};
class Material
{
public:
    Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, const Vector3f &l_color = Vector3f::ZERO,
             double r = 0) : diffuseColor(d_color), refractionIndex(r), specularColor(s_color), lightColor(l_color)
    {
        if (lightColor != Vector3f::ZERO)
            type = ILLUMINANT;
        else if (refractionIndex > 0)
            type = REFRACTIVE;
        else if (specularColor != Vector3f::ZERO)
            type = SPECULAR;
        else
            type = DIFFUSE;
    }

    virtual ~Material() = default;

    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor)
    {
        Vector3f kd;
        if (t.valid() && hit.hasTex)
        {
            Vector2f texCoord = hit.texCoord;
            Vector3f texColor = t(texCoord[0], texCoord[1]);
            kd = texColor;
        }
        else
        {
            kd = this->diffuseColor;
        }
        Vector3f n = hit.getNormal().normalized();
        //Diffuse Shading
        if (noise.valid())
        {
            kd = noise.getColor(ray.getOrigin() + ray.getDirection() * hit.getT());
        }
        Vector3f color = clampedDot(dirToLight, n) * pointwiseDot(lightColor, kd);
        return color;
    }

    Vector3f colorCorrect(const Ray &ray, const Hit &hit)
    {
        if (hit.hasTex)
        {
            Vector2f texCoord = hit.texCoord;
            Vector3f texColor = t(texCoord[0], texCoord[1]);
            return texColor;
        }
        if (noise.valid())
        {
            return noise.getColor(ray.getOrigin() + ray.getDirection() * hit.getT());
        }
        return Vector3f(1.);
    }

    static Vector3f pointwiseDot(const Vector3f &v1, const Vector3f &v2) { return Vector3f(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]); }

    static double clampedDot(const Vector3f &L, const Vector3f &N) { return std::max<double>(Vector3f::dot(L, N), 0.); }
    void loadTexture(const char *filename) { t.load(filename); }
    double getRefractionIndex() const { return refractionIndex; }
    Vector3f getDiffuseColor() const { return diffuseColor; }
    Vector3f getSpecularColor() const { return specularColor; }
    Vector3f getLightColor() const { return lightColor; }
    MeterialType getType() const { return type; }
    void setNoise(const Noise &n) { noise = n; }

protected:
    Vector3f diffuseColor;
    double refractionIndex;
    Vector3f specularColor;
    Vector3f lightColor;
    Texture t;
    Noise noise;
    MeterialType type;
};

#endif // MATERIAL_H
