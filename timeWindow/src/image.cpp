#include "image.h"

Image::Image()
{
  time_stamp_ = std::chrono::steady_clock::now();
}

double Image::getAge() const
{
  auto now = std::chrono::steady_clock::now();
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
      now - time_stamp_);
  return milliseconds.count() * 1e-3;
}

