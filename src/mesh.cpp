#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>

#include "mesh.hpp"

#define SMOOTH (v.size() > 100)

bool Mesh::intersect(const Ray &r, Hit &h, double t_min)
{
    /*bool result = false;
    for (unsigned int i = 0; i < t.size(); i++)
    {
        Triangle triangle(v[t[i][0]],
                          v[t[i][1]], v[t[i][2]], material);
        for (int jj = 0; jj < 3; jj++)
        {
            triangle.normals[jj] = n[t[i][jj]];
        }
        if (texCoord.size() > 0)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                triangle.texCoords[jj] = texCoord[t[i].texID[jj]];
            }
            triangle.hasTex = true;
        }
        result |= triangle.intersect(r, h, t_min);
    }
    return result;*/

    IntersectRecorder func(this, r, h, t_min);
    octree.intersect(r, func);
    return func.result;
}
bool Mesh::intersectTrig(int idx, const Ray &r, Hit &h, double t_min)
{
    bool result = false;
    Triangle triangle(v[t[idx][0]], v[t[idx][1]], v[t[idx][2]], material);

    //some shitty hack
    //change at will
    if (hasNormal)
    {
        for (int jj = 0; jj < 3; jj++)
        {
            triangle.normals[jj] = n[t[idx].nID[jj]];
        }
    }
    else if (SMOOTH)
    {

        for (int jj = 0; jj < 3; jj++)
        {
            triangle.normals[jj] = n[t[idx][jj]];
        }
    }
    else
    {

        for (int jj = 0; jj < 3; jj++)
        {
            triangle.normals[jj] = n[idx];
        }
    }
    if (texCoord.size() > 0)
    {
        for (int jj = 0; jj < 3; jj++)
        {
            triangle.texCoords[jj] = texCoord[t[idx].texID[jj]];
        }
        triangle.hasTex = true;
    }
    result = triangle.intersect(r, h, t_min);
    return result;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material), hasNormal(false)
{
    std::ifstream f;
    f.open(filename);
    if (!f.is_open())
    {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    std::string nTok("vn");
    char slash = '/', space = ' ';
    std::string tok;
    while (1)
    {
        std::getline(f, line);
        if (f.eof())
        {
            break;
        }
        if (line.size() < 3)
        {
            continue;
        }
        if (line.at(0) == '#')
        {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok)
        {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
        }
        else if (tok == fTok)
        {
            Trig trig;
            for (int ii = 0; ii < 3; ii++)
            {
                std::string tem;
                ss >> tem;
                auto first_slash = tem.find(slash);
                auto last_slash = tem.rfind(slash);
                std::replace(tem.begin(), tem.end(), slash, space);
                std::stringstream face(tem);
                if (first_slash == std::string::npos)
                {
                    // f
                    face >> trig[ii];
                    trig[ii]--;
                    trig.texID[ii] = 0;
                    trig.nID[ii] = 0;
                }
                else if (first_slash == last_slash)
                {
                    // f/vt
                    face >> trig[ii] >> trig.texID[ii];
                    trig[ii]--;
                    trig.texID[ii]--;
                    trig.nID[ii] = 0;
                }
                else
                {
                    // f/vt/vn
                    face >> trig[ii] >> trig.texID[ii] >> trig.nID[ii];
                    trig[ii]--;
                    trig.texID[ii]--;
                    trig.nID[ii]--;
                    hasNormal = true;
                }
            }
            t.push_back(trig);
        }
        else if (tok == texTok)
        {
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
            texCoord.push_back(texcoord);
        }
        else if (tok == nTok)
        {
            Vector3f normal;
            ss >> normal[0] >> normal[1] >> normal[2];
            n.push_back(normal);
        }
    }
    f.close();
    if (!hasNormal)
        computeNorm();
    octree.build(*this);
}

void Mesh::computeNorm()
{
    if (SMOOTH)
    {
        n.resize(v.size());
        for (unsigned int ii = 0; ii < t.size(); ii++)
        {
            Vector3f a = v[t[ii][1]] - v[t[ii][0]];
            Vector3f b = v[t[ii][2]] - v[t[ii][0]];
            b = Vector3f::cross(a, b);
            for (int jj = 0; jj < 3; jj++)
            {
                n[t[ii][jj]] += b;
            }
        }
        for (unsigned int ii = 0; ii < v.size(); ii++)
        {
            n[ii] = n[ii] / n[ii].length();
        }
    }
    else
    {
        n.resize(t.size());
        for (unsigned int ii = 0; ii < t.size(); ii++)
        {
            Vector3f a = v[t[ii][1]] - v[t[ii][0]];
            Vector3f b = v[t[ii][2]] - v[t[ii][0]];
            b = Vector3f::cross(a, b);
            n[ii] = b.normalized();
        }
    }
}
