#ifndef GROUP_H
#define GROUP_H

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>

class Group : public Object3D
{

public:
    Group() : objects(0)
    {
    }

    explicit Group(int num_objects) : objects(num_objects)
    {
    }

    ~Group() override
    {
        for (auto object_p : objects)
            delete object_p;
    }

    bool intersect(const Ray &r, Hit &h, double t_min) override
    {
        bool intersected = false;
        for (auto object_p : objects)
        {
            intersected |= object_p->intersect(r, h, t_min);
        }
        return intersected;
    }

    void addObject(int index, Object3D *obj)
    {
        if (index < objects.size())
            objects[index] = obj;
        else
            objects.insert(objects.begin() + index, obj);
    }

    int getGroupSize()
    {
        return objects.size();
    }

private:
    std::vector<Object3D *> objects;
};

#endif
