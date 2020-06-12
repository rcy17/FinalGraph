#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "vector_utils.hpp"
#include "ray_tracer.hpp"

#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    for (int argNum = 1; argNum < argc; ++argNum)
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc < 3)
    {
        cout << "Usage: ./bin/PA5 <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2]; // only bmp is allowed.

    SceneParser parser(argv[1]);
    const auto camera = parser.getCamera();
    Image image(camera->getWidth(), camera->getHeight());
    RayTracer tracer(&parser, 0, true, 0, 0);
#pragma omp parallel for schedule(dynamic, 1)
    for (int x = 0; x < camera->getWidth(); x++)
    {
        fprintf(stderr, "\rprocessing %5d/%-5d", x, camera->getWidth());
        for (int y = 0; y < camera->getHeight(); y++)
        {
            Ray camRay = camera->generateRay(Vector2f(x, y));
            Hit hit;
            auto color = tracer.traceRay(camRay, FLT_MIN, 0, hit);
            image.SetPixel(x, y, color);
        }
    }
    fprintf(stderr, "\rprocessing %5d/%-5d\n", camera->getWidth(), camera->getWidth());
    image.SaveImage(outputFile.c_str());
    return 0;
}
