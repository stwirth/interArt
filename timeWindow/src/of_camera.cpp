
#include "of_camera.h"
#include "of_image.h"

OfCamera::OfCamera()
{
  std::vector<ofVideoDevice> devices = grabber_.listDevices();
  if (devices.size() == 0 )
  {
    throw std::runtime_error("No camera found");
  }
  grabber_.setDeviceID(0);
  grabber_.setDesiredFrameRate(60);
  grabber_.initGrabber(320, 240);
}

void OfCamera::update()
{
  grabber_.update();
}

std::shared_ptr<Image> OfCamera::getImage()
{
  ofPixels pixels = grabber_.getPixels();
  auto image = std::make_shared<OfImage>(pixels);
  return image;
}

bool OfCamera::hasNewFrame()
{
  return grabber_.isFrameNew();
}

