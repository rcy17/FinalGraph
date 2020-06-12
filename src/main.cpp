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

#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    for (int argNum = 1; argNum < argc; ++argNum)
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3)
    {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2]; // only bmp is allowed.

    // DONE: Main RayCasting Logic
    // First, parse the scene using SceneParser.
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    SceneParser parser(argv[1]);
    const auto camera = parser.getCamera();
    const auto group = parser.getGroup();
    Image image(camera->getWidth(), camera->getHeight());
#pragma omp parallel for schedule(dynamic, 1)
    for (int x = 0; x < camera->getWidth(); x++)
    {
        fprintf(stderr, "\rprocessing %5d/%-5d", x, camera->getWidth());
        for (int y = 0; y < camera->getHeight(); y++)
        {
            Ray camRay = camera->generateRay(Vector2f(x, y));
            Hit hit;
            bool intersected = group->intersect(camRay, hit, 0);
            if (intersected)
            {
                auto finalColor = Vector3f::ZERO;
                //auto finalColor = parser.getAmbientLight();
                for (int li = 0; li < parser.getNumLights(); li++)
                {
                    Light *light = parser.getLight(li);
                    Vector3f L, lightColor;
                    float distance;
                    light->getIllumination(camRay.pointAtParameter(hit.getT()), L, lightColor, distance);
                    finalColor += hit.getMaterial()->Shade(camRay, hit, L, lightColor);
                }
                image.SetPixel(x, y, VectorUtils::clamp(finalColor));
            }
            else
            {
                image.SetPixel(x, y, parser.getBackgroundColor());
            }
        }
    }
    fprintf(stderr, "\rprocessing %5d/%-5d\n", camera->getWidth(), camera->getWidth());
    image.SaveImage(outputFile.c_str());
    return 0;
}
