#include "time_line.h"
#include "image.h"
#include <iostream>


void TimeLine::addImage(std::shared_ptr<Image> &image)
{
  images_.push_back(image);
  removeOldImages();
  auto last_image = *(images_.rbegin());
  auto first_image = *(images_.begin());
  std::cout << "timeline has " << images_.size() << " images of ages from "
    << last_image->getAge() << " to " << first_image->getAge() << std::endl;
}

void TimeLine::removeOldImages()
{
  auto it = images_.begin();
  while (it != images_.end()) {
    auto image = *it;
    double age = image->getAge();
    if (age > getTargetHistoryLength()) {
      it = images_.erase(it);
    } else {
      it++;
    }
  }
}

std::shared_ptr<Image> TimeLine::getImageFromPast(double secs) const
{
  auto it = images_.rbegin();
  int index = 0;
  while (it != images_.rend()) {
    auto image = *it;
    double age = image->getAge();
    if (age > secs) {
      return image;
    }
    ++index;
    ++it;
  }
  return nullptr;
}

void TimeLine::setTargetHistoryLength(double secs)
{
  target_history_length_ = secs;
}

double TimeLine::getTargetHistoryLength() const
{
  return target_history_length_;
}

