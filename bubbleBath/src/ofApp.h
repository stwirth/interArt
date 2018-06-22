#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxBox2d.h"
#include "ofxGui.h"

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

  void setPolygonsFromBlobs(const std::vector<ofxCvBlob> &blobs, float scale_factor);

  static ofPolyline cvBlobToSimplePolyline(const ofxCvBlob &blob, float scale_factor);

  void setEdgesFromBlobs(const std::vector<ofxCvBlob> &blobs, float scale_factor);

  void exit();

  void keyPressed(int key);

private:

  void tiltAngleChanged(int &angle);

  ofxKinect kinect_;
  ofxCvColorImage color_image_;

  ofxCvGrayscaleImage gray_image_; // grayscale depth image
  ofxCvGrayscaleImage gray_thresh_near_; // the near thresholded image
  ofxCvGrayscaleImage gray_thresh_far_; // the far thresholded image

  bool show_parameter_gui_;
	ofxPanel parameter_gui_;
	ofParameter<int> near_threshold_;
	ofParameter<int> far_threshold_;
  ofParameter<int> min_radius_;
  ofParameter<int> max_radius_;
  ofParameter<int> max_num_circles_;
  ofParameter<float> line_width_;
  ofParameter<float> density_;
  ofParameter<float> bounce_;
  ofParameter<float> friction_;
  ofParameter<bool> draw_depth_;
  ofParameter<bool> draw_rgb_;
  ofParameter<bool> draw_gray_;
  ofParameter<bool> use_polygons_;
  ofParameter<bool> mirror_image_;

	ofParameter<int> tilt_angle_;

	ofxBox2d box2d_;
	vector<shared_ptr<ofxBox2dPolygon> > polygons_;
  vector<shared_ptr<ofxBox2dEdge> > edges_;
	vector<shared_ptr<FallingCircle> > circles_;

};

