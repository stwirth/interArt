#include "frame.h"
#include "image.h"
#include "time_line.h"

#include <memory>
#include <iostream>
#include <cstdlib>

Frame::Frame(const TimeLine &time_line) : time_line_(time_line)
{
  std::srand(std::time(0));
}

void Frame::draw(int x, int y, int width, int height)
{
  if (is_open_) {
    std::shared_ptr<Image> image = time_line_.getImageFromPast(time_offset_);
    if (image) {
      image->draw(x, y, width, height);
      std::cout << "Painting image " << image.get() << " with age " << image->getAge() <<  std::endl;
    }
  } else {
    // TODO paint color
  }
}

void Frame::open()
{
  time_offset_ = 1.0 * std::rand() / RAND_MAX * time_line_.getTargetHistoryLength();
  is_open_ = true;
}

bool Frame::isOpen() const
{
  return is_open_;
}

void Frame::close()
{
  is_open_ = false;
}

