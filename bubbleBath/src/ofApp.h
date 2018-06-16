#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxBox2d.h"

class ofApp : public ofBaseApp{

public:

  class FallingCircle : public ofxBox2dCircle {

   public:
    void setColor(int r, int g, int b)
    {
      r_ = r;
      g_ = g;
      b_ = b;
    }

    void setRandomColor()
    {
      r_ = ofRandom(0, 255);
      g_ = ofRandom(0, 255);
      b_ = ofRandom(0, 255);
    }

    void draw() {
      if(!isBody()) return;

      ofPushMatrix();
      ofPushStyle();
      ofTranslate(getPosition().x, getPosition().y, 0);
      ofRotate(getRotation(), 0, 0, 1);
      ofSetColor(r_, g_, b_);
      ofDrawCircle(0, 0, getRadius());

      ofEnableAlphaBlending();
      if(isSleeping()) {
        ofSetColor(255, 100);
        ofDrawCircle(0, 0, getRadius());
      }
      ofPopStyle();
      ofPopMatrix();
    }
   private:
    int r_;
    int g_;
    int b_;
  };

  void setup();
  void update();
  void draw();
  void exit();

  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void mouseEntered(int x, int y);
  void mouseExited(int x, int y);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);

private:

  ofxKinect kinect_;
  ofxCvColorImage color_image_;

  ofxCvGrayscaleImage gray_image_; // grayscale depth image
  ofxCvGrayscaleImage gray_thresh_near_; // the near thresholded image
  ofxCvGrayscaleImage gray_thresh_far_; // the far thresholded image

  ofxCvContourFinder contour_finder_;

	int near_threshold_;
	int far_threshold_;
  int radius_max_;

	int tilt_angle_;

	ofxBox2d box2d_;           // the box2d world
	vector<shared_ptr<ofxBox2dEdge> > edge_lines_;   // the box2d edge/line shape (min 2 points)
	vector<shared_ptr<FallingCircle> > circles_;
  int max_num_circles_;

};

