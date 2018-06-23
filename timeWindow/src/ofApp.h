#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxGui.h"

#include "camera.h"
#include "time_window.h"

class ofApp : public ofBaseApp{

public:

  void setup();

  void update();

  void draw();

  void exit();

  void keyPressed(int key);

private:

  bool show_parameter_gui_;
	ofxPanel parameter_gui_;

  std::shared_ptr<Camera> camera_;
  TimeWindow time_window_;

};

