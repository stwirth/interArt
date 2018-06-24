
#include "of_kinect_camera.h"
#include "of_image.h"

OfKinectCamera::OfKinectCamera()
{
	kinect_.init();
}

void OfKinectCamera::update()
{
  kinect_.update();
}

std::shared_ptr<Image> OfKinectCamera::getImage()
{
  ofPixels pixels = kinect_.getPixels();
  auto image = std::make_shared<OfImage>(pixels);
  return image;
}

bool OfKinectCamera::hasNewFrame()
{
  return kinect_.isFrameNew();
}

