#include "ofApp.h"
#include "of_camera.h"
#include "of_kinect_camera.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetVerticalSync(true);

	parameter_gui_.setup("Parameters");
  show_parameter_gui_ = false;

  //camera_.reset(new OfCamera());
  camera_.reset(new OfKinectCamera());

	ofSetFrameRate(60);

	ofBackground(0, 0, 0);
}

//--------------------------------------------------------------
void ofApp::update(){

	camera_->update();
  time_window_.update();

  if (camera_->hasNewFrame()) {
    std::shared_ptr<Image> img = camera_->getImage();
    time_window_.addImage(img);
  }
}

//--------------------------------------------------------------
void ofApp::draw(){

  time_window_.draw(0, 0, ofGetWidth(), ofGetHeight());

  if (show_parameter_gui_) {
    parameter_gui_.draw();
  }
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {

    case 'p':
		case 'm':
      show_parameter_gui_ = !show_parameter_gui_;
			break;

		case ' ':
      ofToggleFullscreen();
			break;

	}
}

void ofApp::exit() {
}


