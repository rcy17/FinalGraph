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

Image render(const ArgParser &parser, SceneParser *scene, int height, int width, int offset, int y_range)
{
    auto camera = scene->getCamera();
    camera->setSize(width, height);
    Image image(camera->getWidth(), y_range);
    Tracer *tracer;
    switch (parser.type)
    {
    case RT:
        tracer = new RayTracer(scene, parser.bounces, parser.shadows, parser.refractions);
        break;
    case PT:
        tracer = new PathTracer(scene, parser.bounces, parser.shadows, parser.refractions);
        break;
    }

    const double t_min = camera->getDistance() + EPSILON;

#pragma omp parallel for schedule(dynamic, 1)
    for (int _y = 0; _y < y_range; _y++)
    {
        fprintf(stderr, "\rprocessing %5d/%-5d", _y, y_range);
        for (int x = 0; x < image.Width(); x++)
        {
            int y = _y + offset;
            bool debug = false;
            if (y == 1 && x == 100 && parser.debug)
            {
                debug = true;
            }
            Vector3f color;
            for (int sy = 0; sy < 2; sy++)
                for (int sx = 0; sx < 2; sx++)
                {
                    unsigned short seed[3] = {
                        y,
                        static_cast<unsigned short>(y * x),
                        static_cast<unsigned short>(y * x * y)};
                    double r1 = 2 * erand48(seed), dx = r1 < 1 ? sqrt(r1) : 2 - sqrt(2 - r1);
                    double r2 = 2 * erand48(seed), dy = r2 < 1 ? sqrt(r2) : 2 - sqrt(2 - r2);
                    Ray camRay = camera->generateRay(Vector2f(x + dx / 2 + sx, y + dy / 2 + sy));
                    for (int i = 0; i < parser.spp; i++)
                    {
                        color += tracer->traceRay(camRay, t_min, 0, seed, 1.f, debug);
                    }
                }
            image.SetPixel(x, _y, VectorUtils::clamp(color / parser.spp / 4));
        }
    }
    delete tracer;
    fprintf(stderr, "\rprocessing %5d/%-5d\n", y_range, y_range);
    return image;
}

Image merge(const ArgParser &parser)
{
    Image image(parser.width, parser.height);
    image.Merge(parser.segments);
    return image;
}

int main(int argc, char *argv[])
{
    for (int argNum = 1; argNum < argc; ++argNum)
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }
    ArgParser parser(argc, argv);

    SceneParser scene(parser.input_file);
    int height, width;
    if (parser.set_size)
        height = parser.height, width = parser.width;
    else
        height = scene.getCamera()->getHeight(), width = scene.getCamera()->getHeight();
    if (parser.jitter)
        height *= 3, width *= 3;

    int offset = parser.offset;
    int y_range = (parser.size ? parser.size : height - offset);
    bool segment = y_range != height;

    auto image = parser.segments.empty() ? render(parser, &scene, height, width, offset, y_range) : merge(parser);

    image.SaveImage(parser.output_file, parser.gamma);

    return 0;
}
