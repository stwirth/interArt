#pragma once

#include <chrono>

class Image
{

 public:

  Image();

  virtual ~Image() {}

  /**
   * Draw the image at specified position with given dimensions.
   */
  virtual void draw(int x, int y, int width, int height) = 0;

  /**
   * Age in seconds.
   */
  double getAge() const;

 private:

  std::chrono::time_point<std::chrono::steady_clock> time_stamp_;

};

