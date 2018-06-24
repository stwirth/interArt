#pragma once

#include "camera.h"
#include "ofxKinect.h"
#include <memory>

class OfKinectCamera : public Camera
{
 public:

  OfKinectCamera();

  void update();

  virtual std::shared_ptr<Image> getImage();

  virtual bool hasNewFrame();

 private:

  ofxKinect kinect_;

};

