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
    virtual Ray generateRay(const Vector2f &point) = 0;
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
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction, const Vector3f &up,
                      int imgW, int imgH, double angle, double dis) : Camera(center, direction, up, imgW, imgH, dis)
    {
        // angle is in radian.
        f_x = f_y = height / (2 * tan(angle / 2));

        // f_y = height / (2 * tan(angle / 2));
        this->angle = angle;
    }

    void setSize(int width, int height)
    {
        this->width = width, this->height = height;
        f_x = height / (2 * tan(angle / 2));
        f_y = f_x;
        //f_y = height / (2 * tan(angle / 2));
    }

    Ray generateRay(const Vector2f &point) override
    {
        Vector3f ray((point.x() - width / 2) / f_x, (point.y() - height / 2) / f_y, 1);
        Matrix3f transform(horizontal, up, direction);
        ray = transform * ray;
        return Ray(center, ray.normalized());
    }

private:
    double f_x, f_y;
    double angle;
};

#endif //CAMERA_H
