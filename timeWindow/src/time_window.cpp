#include "time_window.h"

#include <algorithm>
#include <iostream>
#include <cstdlib>

TimeWindow::TimeWindow()
{
  num_rows_ = 3;
  num_cols_ = 4;
  allocateFrames();
  last_switch_time_ = std::chrono::steady_clock::now();
  std::srand(std::time(0));
}

void TimeWindow::setNumRows(int rows)
{
  if (rows > 0) {
    num_rows_ = rows;
    allocateFrames();
  }
}

void TimeWindow::setNumCols(int cols)
{
  if (cols > 0) {
    num_cols_ = cols;
    allocateFrames();
  }
}

void TimeWindow::allocateFrames()
{
  int num_frames = num_rows_ * num_cols_;
  while (frames_.size() < num_frames) {
    auto frame = std::make_shared<Frame>(time_line_);
    frames_.push_back(std::make_shared<Frame>(time_line_));
  }
  while (frames_.size() > num_frames) {
    frames_.pop_back();
  }
}

void TimeWindow::update()
{
  double secs = getSecondsSinceLastSwitch();
  if (secs > 4.0)
  {
    int frame_index = std::rand() % frames_.size();
    if (frames_[frame_index]->isOpen()) {
      frames_[frame_index]->close();
    } else {
      frames_[frame_index]->open();
    }
    last_switch_time_ = std::chrono::steady_clock::now();
  }
}

double TimeWindow::getSecondsSinceLastSwitch() const
{
  auto now = std::chrono::steady_clock::now();
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
      now - last_switch_time_);
  return milliseconds.count() * 1e-3;
}

void TimeWindow::draw(int x, int y, int width, int height)
{
  int frame_width = width / num_cols_;
  int frame_height = height / num_rows_;
  int frame_x = 0;
  int frame_y = 0;
  for (auto frame : frames_) {
    frame->draw(frame_x, frame_y, frame_width, frame_height);
    frame_x += frame_width;
    if (frame_x >= width) {
      frame_x = 0;
      frame_y += frame_height;
    }
  }
}

void TimeWindow::addImage(std::shared_ptr<Image> &image)
{
  time_line_.addImage(image);
}

void TimeWindow::setHistoryLength(double secs)
{
  if (secs > 0.0) {
    time_line_.setTargetHistoryLength(secs);
  }
}

void TimeWindow::setFractionOpen(double fraction)
{
  if (fraction >= 0.0 && fraction <= 1.0) {
    std::vector<int> indices;
    for (int i = 0; i < frames_.size(); ++i) {
      indices.push_back(i);
    }
    std::random_shuffle(indices.begin(), indices.end());
    int num_open = fraction * frames_.size();
    int index_used = 0;
    while (index_used < num_open) {
      frames_[indices[index_used]]->open();
      ++index_used;
    }
    while (index_used < frames_.size()) {
      frames_[indices[index_used]]->close();
      ++index_used;
    }
  }
}

void TimeWindow::setOpenDuration(double secs) {
  open_duration_ = secs;
}

