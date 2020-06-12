#ifndef LIGHT_H
#define LIGHT_H

#include <vecmath.h>

#include "object3d.hpp"

class Light
{
public:
    Light() = default;

    virtual ~Light() = default;

    virtual void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col, float &distanceToLight) const = 0;
};

class DirectionalLight : public Light
{
public:
    DirectionalLight() = delete;

    DirectionalLight(const Vector3f &d, const Vector3f &c)
    {
        direction = d.normalized();
        color = c;
    }

    ~DirectionalLight() override = default;
    ///@param p unsed in this function
    ///@param distanceToLight not well defined because it's not a point light
    virtual void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col, float &distanceToLight) const
    {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = -direction;
        col = color;
        distanceToLight = FLT_MAX;
    }

private:
    Vector3f direction;
    Vector3f color;
};

class PointLight : public Light
{
public:
    PointLight(const Vector3f &p, const Vector3f &c, float fall)
    {
        position = p;
        color = c;
        falloff = fall;
    }

    ~PointLight() override = default;

    virtual void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col, float &distanceToLight) const
    {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = (position - p);
        distanceToLight = dir.length();
        dir = dir / dir.length();
        col = color / (1 + falloff * distanceToLight * distanceToLight);
    }

private:
    float falloff;
    Vector3f position;
    Vector3f color;
};

#endif // LIGHT_H
