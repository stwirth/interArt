#pragma once

class TimeLine;

class Frame
{

 public:

  Frame(const TimeLine &timeline);

  void update();

  void draw(int x, int y, int width, int height);

  void open();

  bool isOpen() const;

  void close();

 private:

  const TimeLine &time_line_;
  double time_offset_;
  bool is_open_;

};

