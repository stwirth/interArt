#pragma once

#include <memory>
#include <map>
#include <chrono>
#include <deque>

class Image;

class TimeLine
{

 public:

  void addImage(std::shared_ptr<Image> &image);

  /**
   * @returns null_ptr if secs is outside time line.
   */
  std::shared_ptr<Image> getImageFromPast(double secs) const;

  /**
   * Set how many seconds the timeline should accumulate.
   */
  void setTargetHistoryLength(double secs);

  double getTargetHistoryLength() const;

 private:

  void removeOldImages();

  double target_history_length_ = 60.0; // seconds

  std::deque<std::shared_ptr<Image>> images_;

};

