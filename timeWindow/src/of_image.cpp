#include "of_image.h"

#include <iostream>

OfImage::OfImage(const ofPixels &pixels)
{
  image_.setFromPixels(pixels);
}

void OfImage::draw(int x, int y, int width, int height)
{
  image_.draw(x, y, width, height);
}

