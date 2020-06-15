#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include "bitmap_image.hpp"
#include "Vector3f.h"
///@brief helper class that stores a texture and faciliates lookup
///assume 4byte RGBA image data
class Texture
{
public:
  Texture();
  bool valid() const;
  void load(const char *filename);
  void operator()(int x, int y, unsigned char *color) const;
  ///@param x assumed to be between 0 and 1
  Vector3f operator()(double x, double y) const;
  ~Texture();

private:
  bitmap_image *bimg;
  int width, height;
};
#endif
