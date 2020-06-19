#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <cmath>

class Camera
{
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH, double dis)
    {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up).normalized();
        this->up = Vector3f::cross(this->horizontal, this->direction).normalized();
        this->width = imgW;
        this->height = imgH;
        this->distance = dis;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point, unsigned short *seed = nullptr) const = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    double getDistance() const { return distance; }
    virtual void setSize(int width, int height) { this->width = width, this->height = height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
    // Optional parameters
    double distance;
};

// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera
{

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW,
                      int imgH, double angle, double dis) : Camera(center, direction, up, imgW, imgH, dis)
    {
        // angle is in radian.
        f = 1 / (2 * tan(angle / 2));
        this->angle = angle;
    }

    void setSize(int width, int height)
    {
        this->width = width, this->height = height;
    }

    Ray generateRay(const Vector2f &point, unsigned short *seed = nullptr) const override
    {
        // The two division to height is not BUG, if you have any question, try to divide to width for x
        // It only matters when height != width
        Vector3f ray((point.x() - width * 0.5) / height, (point.y() - height * 0.5) / height, f);
        Matrix3f transform(horizontal, up, direction);
        ray = transform * ray;
        return Ray(center, ray.normalized());
    }

protected:
    double f;
    double angle;
};

class DepthCamera : public PerspectiveCamera
{
public:
    DepthCamera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH, double angle,
                double dis, double aperture) : PerspectiveCamera(center, direction, up, imgW, imgH, angle, dis), aperture(aperture)
    {
    }

    Ray generateRay(const Vector2f &point, unsigned short *seed) const override
    {
        Vector3f ray((point.x() - width * 0.5) / height, (point.y() - height * 0.5) / height, f);
        Matrix3f transform(horizontal, up, direction);
        ray = transform * ray;
        Vector3f jitter = transform * Vector3f(erand48(seed) - 0.5, erand48(seed) - 0.5, 0) * aperture / 100;
        return Ray(center + jitter, (ray * (center.length() / f) - jitter).normalized());
    }

protected:
    double aperture;
};

#endif //CAMERA_H
