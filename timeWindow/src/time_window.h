#pragma once

#include "frame.h"
#include "time_line.h"

#include <vector>
#include <memory>

class TimeWindow
{
 public:

  TimeWindow();

  void setNumRows(int rows);

  void setNumCols(int cols);

  void update();

  void draw(int x, int y, int width, int height);

  void addImage(std::shared_ptr<Image> &image);

  void setHistoryLength(double secs);

  void setFractionOpen(double fraction);

  void setOpenDuration(double secs);

 private:

  double getSecondsSinceLastSwitch() const;

  void allocateFrames();

  int num_rows_;
  int num_cols_;

  std::vector<std::shared_ptr<Frame>> frames_;

  TimeLine time_line_;

  double open_duration_;
  std::chrono::time_point<std::chrono::steady_clock> last_switch_time_;

};

