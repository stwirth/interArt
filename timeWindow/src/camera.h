#pragma once

#include "ofVideoGrabber.h"

#include <memory>

class Image;

class Camera
{

 public:

  virtual ~Camera() {};

  virtual void update() {};

  virtual std::shared_ptr<Image> getImage() = 0;

  virtual bool hasNewFrame() = 0;

};

