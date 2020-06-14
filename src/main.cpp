#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "argparser.hpp"
#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "vector_utils.hpp"
#include "ray_tracer.hpp"
#include "path_tracer.hpp"

#include <string>

using namespace std;

void test()
{

    Sphere s({0, 0, 0}, 1, nullptr);
    {
        Ray r({0, 0, 2}, {0, 0, -1});
        Hit h;
        Vector3f dir;
        transmittedDirection({0, 0, 1}, {0, 0, -1}, 1, 1.3, dir);
        s.intersect(r, h, 0);
    }

    Ray r({0, 0, 0}, {0, 0, -1});
    Hit h;
    s.intersect(r, h, 0);
    Vector3f dir;
    transmittedDirection({0, 0, -1}, {0, 0, -1}, 1.3, 1, dir);

    exit(0);
}

int main(int argc, char *argv[])
{
    //test();
    for (int argNum = 1; argNum < argc; ++argNum)
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }
    ArgParser parser(argc, argv);

    SceneParser scene(parser.input_file);
    auto camera = scene.getCamera();
    if (parser.set_size)
        camera->setSize(parser.width, parser.height);
    if (parser.jitter)
        camera->setSize(camera->getWidth() * 3, camera->getHeight() * 3);
    Image image(camera->getWidth(), camera->getHeight());
    Tracer *tracer;
    switch (parser.type)
    {
    case RT:
        tracer = new RayTracer(&scene, parser.bounces, parser.shadows, parser.refractions);
        break;
    case PT:
        tracer = new PathTracer(&scene, parser.bounces, parser.shadows, parser.refractions);
        break;
    }

#pragma omp parallel for schedule(dynamic, 1)
    for (int y = 0; y < image.Height(); y++)
    {
        fprintf(stderr, "\rprocessing %5d/%-5d", y, image.Width());
        for (int x = 0; x < image.Width(); x++)
        {
            bool debug = false;
            if (y == 32 && x == 17 && 0)
            {
                debug = true;
            }
            Vector3f color;
            unsigned short seed[3] = {y, y * x, static_cast<unsigned short>(y * x * y)};
            for (int i = 0; i < parser.spp; i++)
            {
                if (parser.jitter)
                {
                    auto dx = erand48(seed);
                    auto dy = erand48(seed);
                    Ray camRay = camera->generateRay(Vector2f(x - 0.5 + dx, y - 0.5 + dy));
                    color += tracer->traceRay(camRay, 0.f, 0, seed, 1.f, debug);
                }
                else
                {
                    Ray camRay = camera->generateRay(Vector2f(x, y));
                    color += tracer->traceRay(camRay, 0.f, 0, seed, 1.f, debug);
                }
            }
            image.SetPixel(x, y, color / parser.spp);
        }
    }
    fprintf(stderr, "\rprocessing %5d/%-5d\n", image.Width(), image.Width());
    if (parser.filter)
    {
        image.GaussianBlur();
        Image result(image.Width() / 3, image.Height() / 3);
        image.DownSampling(&result);
        result.SaveImage(parser.output_file);
    }
    else
    {
        image.SaveImage(parser.output_file);
    }
    return 0;
}
