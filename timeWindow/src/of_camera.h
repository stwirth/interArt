#pragma once

#include "camera.h"
#include <memory>

class OfCamera : public Camera
{

 public:

  OfCamera();

  void update();

  virtual std::shared_ptr<Image> getImage();

  virtual bool hasNewFrame();

 private:

  ofVideoGrabber grabber_;

};

