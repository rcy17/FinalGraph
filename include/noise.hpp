#ifndef NOISE_H
#define NOISE_H
#include "vecmath.h"
class Noise
{
public:
	Noise();
	Noise(int _octaves,
		  const Vector3f &color1 = Vector3f::ZERO,
		  const Vector3f &color2 = Vector3f(1, 1, 1),
		  double freq = 1, double amp = 1);
	Vector3f getColor(const Vector3f &pos) const;
	bool valid() const;

	Noise(const Noise &n);

	Vector3f color[2];
	int octaves;
	double frequency;
	double amplitude;
	bool init;
};

#endif