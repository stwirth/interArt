#include "ofApp.h"

static bool shouldRemoveBelowScreen(shared_ptr<ofxBox2dBaseShape> shape)
{
  ofApp::FallingCircle *fc = dynamic_cast<ofApp::FallingCircle*>(shape.get());
  if (fc)
  {
    return fc->getPosition().y - fc->getRadius() > ofGetHeight();
  }
  return true;
}

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetVerticalSync(true);

	parameter_gui_.setup("Parameters");
  parameter_gui_.add(near_threshold_.set("near_threshold (n/N)", 255, 0, 255));
  parameter_gui_.add(far_threshold_.set("far_threshold (f/F)", 178, 0, 255));
  parameter_gui_.add(min_radius_.set("min_radius", 5, 1, 200));
  parameter_gui_.add(max_radius_.set("max_radius (r/R)", 100, 5, 200));
  parameter_gui_.add(max_num_circles_.set("max_num_circles (+/-)", 200, 5, 500));
  parameter_gui_.add(line_width_.set("line_width", 2.0, 0.5, 10.0));
  parameter_gui_.add(density_.set("density", 3.0, 0.0, 10.0));
  parameter_gui_.add(bounce_.set("bounce", 0.53, 0.0, 1.0));
  parameter_gui_.add(friction_.set("friction", 0.1, 0.0, 10.0));
  parameter_gui_.add(tilt_angle_.set("tilt_angle", 0, -30, 30));
  parameter_gui_.add(draw_depth_.set("draw_depth", false));
  parameter_gui_.add(draw_rgb_.set("draw_rgb", false));
  parameter_gui_.add(draw_gray_.set("draw_gray", false));
  parameter_gui_.add(use_polygons_.set("use_polygons", false));
  parameter_gui_.add(mirror_image_.set("mirro_image", true));
  show_parameter_gui_ = false;

	tilt_angle_.addListener(this, &ofApp::tiltAngleChanged);

	box2d_.init();
	box2d_.setGravity(0, 10);
	//box2d_.createGround();
	box2d_.setFPS(60.0);
	//box2d_.registerGrabbing();

	// enable depth->video image calibration
	kinect_.setRegistration(true);
	kinect_.init();

	color_image_.allocate(kinect_.width, kinect_.height);
	gray_image_.allocate(kinect_.width, kinect_.height);
	gray_thresh_near_.allocate(kinect_.width, kinect_.height);
	gray_thresh_far_.allocate(kinect_.width, kinect_.height);

	ofSetFrameRate(60);

	// zero the tilt on startup
	kinect_.setCameraTiltAngle(tilt_angle_);

	ofBackground(0, 0, 0);
}

//--------------------------------------------------------------
void ofApp::update(){

	box2d_.update();
	kinect_.update();

  if(circles_.size() < max_num_circles_) {
    shared_ptr<FallingCircle> circle(new FallingCircle());
		circle->setPhysics(density_, bounce_, friction_);
    circle->setRandomColor();
    float radius_diff = max_radius_ - min_radius_;
    float this_max_radius = 0.5 * radius_diff + sin(0.1 * ofGetElapsedTimef()) * 0.5 * radius_diff;
		float radius = ofRandom(min_radius_, this_max_radius);
    float x = ofRandomWidth();
    float y = -radius - 50;
		circle->setup(box2d_.getWorld(), x, y, radius);
		circles_.push_back(circle);
  }

	// remove shapes offscreen
	ofRemove(circles_, shouldRemoveBelowScreen);

	// there is a new frame and we are connected
	if(kinect_.isFrameNew()) {

		// load grayscale depth image from the kinect source
		gray_image_.setFromPixels(kinect_.getDepthPixels());

    if (mirror_image_) {
      bool flip_horizontally = true;
      bool flip_vertically = false;
      gray_image_.mirror(flip_vertically, flip_horizontally);
    }

    gray_thresh_near_ = gray_image_;
    gray_thresh_far_ = gray_image_;
    gray_thresh_near_.threshold(near_threshold_, true);
    gray_thresh_far_.threshold(far_threshold_);
    cvAnd(gray_thresh_near_.getCvImage(), gray_thresh_far_.getCvImage(), gray_image_.getCvImage(), NULL);

		// update the cv images
		gray_image_.flagImageChanged();

    const int min_area = 10;
    const int max_area = kinect_.width * kinect_.height / 2.0;
    const int max_num_blobs = 10;
    const bool find_holes = false;
    const bool approximate = true;
    ofxCvContourFinder contour_finder;
		contour_finder.findContours(gray_image_, min_area, max_area, max_num_blobs, find_holes, approximate);

    // scale factor to get blobs on whole screen
    float x_scale = 1.0f * ofGetWidth() / gray_image_.width;
    float y_scale = 1.0f * ofGetHeight() / gray_image_.height;
    float scale_factor = std::max(x_scale, y_scale);

    if (use_polygons_) {
      setPolygonsFromBlobs(contour_finder.blobs, scale_factor);
    } else {
      setEdgesFromBlobs(contour_finder.blobs, scale_factor);
    }
  }
}

void ofApp::setPolygonsFromBlobs(const std::vector<ofxCvBlob> &blobs, float scale_factor) {
  polygons_.clear();
  for (size_t i = 0; i < blobs.size(); ++i) {
    ofPolyline polyline = cvBlobToSimplePolyline(blobs[i], scale_factor);
    if (polyline.getVertices().size() > 2) {
      auto polygon = std::make_shared<ofxBox2dPolygon>();
      polygon->addVertices(polyline.getVertices());
      polygon->setPhysics(0.0, 0.5, 0.5);
      polygon->triangulatePoly();
      polygon->create(box2d_.getWorld());
      polygons_.push_back(polygon);
    }
  }
}

ofPolyline ofApp::cvBlobToSimplePolyline(const ofxCvBlob &blob, float scale_factor) {
  ofPolyline poly;
  for (size_t p = 0; p < blob.pts.size(); ++p) {
    const ofDefaultVec3 pt = blob.pts[p];
    poly.addVertex(pt.x * scale_factor, pt.y * scale_factor);
  }
  poly.simplify();
  return poly.getResampledBySpacing(25);
}

void ofApp::setEdgesFromBlobs(const std::vector<ofxCvBlob> &blobs, float scale_factor) {
  edges_.clear();
  for (size_t i = 0; i < blobs.size(); ++i) {
    ofPolyline polyline = cvBlobToSimplePolyline(blobs[i], scale_factor);
    if (polyline.getVertices().size() > 2) {
      auto edge = std::make_shared<ofxBox2dEdge>();
      edge->addVertices(polyline.getVertices());
      edge->setPhysics(0.0, 0.5, 0.5);
      edge->create(box2d_.getWorld());
      edges_.push_back(edge);
    }
  }
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(255, 255, 255);

	for (size_t i = 0; i < circles_.size(); i++)
  {
		ofFill();
		circles_[i]->draw();
	}

  ofPushStyle();
  ofFill();
  ofSetLineWidth(line_width_);
  ofSetHexColor(0xffffff);
	for (size_t i = 0; i < polygons_.size(); i++)
  {
		polygons_[i]->draw();
	}

	for (size_t i = 0; i < edges_.size(); i++)
  {
		edges_[i]->draw();
	}
  ofPopStyle();

  if (draw_depth_) {
    kinect_.drawDepth(10, 10, 400, 300);
  }
  if (draw_rgb_) {
    kinect_.draw(420, 10, 400, 300);
  }
  if (draw_gray_) {
    gray_image_.draw(10, 320, 400, 300);
  }
  if (show_parameter_gui_) {
    parameter_gui_.draw();
  }

}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {

		case '+':
      max_num_circles_ += 10;
      break;

		case '-':
      max_num_circles_ -= 10;
      if (max_num_circles_ < 10) max_num_circles_ = 10;
      break;

		case 'F':
			far_threshold_++;
			if (far_threshold_ > 255) far_threshold_ = 255;
			break;

		case 'f':
			far_threshold_ --;
			if (far_threshold_ < 0) far_threshold_ = 0;
			break;

    case 'p':
		case 'm':
      show_parameter_gui_ = !show_parameter_gui_;
			break;

    case 'N':
			near_threshold_ ++;
			if (near_threshold_ > 255) near_threshold_ = 255;
			break;

		case 'n':
			near_threshold_ --;
			if (near_threshold_ < 0) near_threshold_ = 0;
			break;

		case 'w':
			kinect_.enableDepthNearValueWhite(!kinect_.isDepthNearValueWhite());
			break;

		case ' ':
      ofToggleFullscreen();
			break;

		case 'r':
      max_radius_ -= 5;
      if (max_radius_ < 5) max_radius_ = 5;
      break;

		case 'R':
      max_radius_ += 5;
      break;

		case '1':
			kinect_.setLed(ofxKinect::LED_GREEN);
			break;

		case '2':
			kinect_.setLed(ofxKinect::LED_YELLOW);
			break;

		case '3':
			kinect_.setLed(ofxKinect::LED_RED);
			break;

		case '4':
			kinect_.setLed(ofxKinect::LED_BLINK_GREEN);
			break;

		case '5':
			kinect_.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
			break;

		case '0':
			kinect_.setLed(ofxKinect::LED_OFF);
			break;

		case OF_KEY_UP:
			tilt_angle_++;
			if(tilt_angle_>30) tilt_angle_=30;
			kinect_.setCameraTiltAngle(tilt_angle_);
			break;

		case OF_KEY_DOWN:
			tilt_angle_--;
			if(tilt_angle_<-30) tilt_angle_=-30;
			break;
	}
}

void ofApp::exit() {
  kinect_.setCameraTiltAngle(0); // zero the tilt on exit
  kinect_.close();
}

void ofApp::tiltAngleChanged(int &angle) {
  kinect_.setCameraTiltAngle(tilt_angle_);
}


