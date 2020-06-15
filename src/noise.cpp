#include "noise.hpp"
#include "perlin_noise.hpp"

Vector3f Noise::getColor(const Vector3f &pos) const
{
	//Fill in this function  ONLY.
	//INTERPOLATE BETWEEN TWO COLORS BY WEIGHTED AVERAGE
	auto N = PerlinNoise::octaveNoise(pos, octaves);
	auto M = sin(frequency * pos.x() + amplitude * N);
	return color[0] + (M + 1) / 2 * (color[1] - color[0]);
}

Noise::Noise(int _octaves, const Vector3f &color1,
			 const Vector3f &color2, double freq, double amp) : octaves(_octaves), frequency(freq), amplitude(amp)
{
	color[0] = color1;
	color[1] = color2;
	init = true;
}

Noise::Noise(const Noise &n) : octaves(n.octaves), frequency(n.frequency),
							   amplitude(n.amplitude), init(n.init)
{
	color[0] = n.color[0];
	color[1] = n.color[1];
}

bool Noise::valid() const
{
	return init;
}

Noise::Noise() : octaves(0), init(false)
{
}
