#pragma once

#include "image.h"
#include "ofPixels.h"
#include "ofImage.h"

class OfImage : public Image
{

 public:

  OfImage(const ofPixels &texture);

  virtual void draw(int x, int y, int width, int height);

 private:

  ofImage image_;

};

