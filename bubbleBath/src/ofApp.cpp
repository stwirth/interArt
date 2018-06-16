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

	box2d_.init();
	box2d_.setGravity(0, 10);
	//box2d_.createGround();
	box2d_.setFPS(60.0);
	//box2d_.registerGrabbing();

	// enable depth->video image calibration
	kinect_.setRegistration(true);
	kinect_.init();

	// print the intrinsic IR sensor values
	if(kinect_.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect_.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect_.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect_.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect_.getZeroPlaneDistance() << "mm";
	}

	color_image_.allocate(kinect_.width, kinect_.height);
	gray_image_.allocate(kinect_.width, kinect_.height);
	gray_thresh_near_.allocate(kinect_.width, kinect_.height);
	gray_thresh_far_.allocate(kinect_.width, kinect_.height);

	near_threshold_ = 255;
	far_threshold_ = 178;
  radius_max_ = 50;
  max_num_circles_ = 200;

	ofSetFrameRate(60);

	// zero the tilt on startup
	tilt_angle_ = 0;
	kinect_.setCameraTiltAngle(tilt_angle_);

	ofBackground(0, 0, 0);
}

//--------------------------------------------------------------
void ofApp::update(){

	box2d_.update();
	kinect_.update();

  const int max_num_circles = 100;
  if (circles_.size() < max_num_circles)
  {
    shared_ptr<FallingCircle> circle(new FallingCircle());
    // density, bounca, friction
		circle->setPhysics(3.0, 0.53, 0.1);
    circle->setRandomColor();
		float radius = ofRandom(4, radius_max_);	// a random radius 4px - 20px
    float x = ofRandomWidth();
    float y = -radius;
		circle->setup(box2d_.getWorld(), x, y, radius);
		circles_.push_back(circle);
  }

	// remove shapes offscreen
	ofRemove(circles_, shouldRemoveBelowScreen);


	// there is a new frame and we are connected
	if(kinect_.isFrameNew()) {

		// load grayscale depth image from the kinect source
		gray_image_.setFromPixels(kinect_.getDepthPixels());

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
		contour_finder_.findContours(gray_image_, min_area, max_area, max_num_blobs, find_holes, approximate);

    edge_lines_.clear();

    // TODO resize to screen
    float x_scale = 1.0f * ofGetWidth() / gray_image_.width;
    float y_scale = 1.0f * ofGetHeight() / gray_image_.height;
    float scale = std::max(x_scale, y_scale);
    for (size_t i = 0; i < contour_finder_.blobs.size(); ++i)
    {
      const ofxCvBlob &blob = contour_finder_.blobs[i];
      vector<ofVec2f> vertices(blob.pts.size());
      for (size_t p = 0; p < blob.pts.size(); ++p)
      {
        const ofDefaultVec3 pt = blob.pts[p];
        vertices[p].x = pt.x * scale;
        vertices[p].y = pt.y * scale;
      }
      shared_ptr<ofxBox2dEdge> edge(new ofxBox2dEdge());
      edge->addVertexes(vertices);
      edge->setPhysics(0.0, 0.5, 0.5);
      edge->create(box2d_.getWorld());
      edge_lines_.push_back(edge);
    }
  }
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetColor(255, 255, 255);

  // draw from the live kinect
  //kinect_.drawDepth(10, 10, 400, 300);
  //kinect_.draw(0, 0, 400, 300);

  //gray_image_.draw(10, 320, 400, 300);
  //contour_finder_.draw(10, 320, 400, 300);


	// draw instructions
  /*
	ofSetColor(255, 255, 255);
	stringstream reportStream;

   if(kinect_.hasAccelControl()) {
        reportStream << "accel is: " << ofToString(kinect_.getMksAccel().x, 2) << " / "
        << ofToString(kinect_.getMksAccel().y, 2) << " / "
        << ofToString(kinect_.getMksAccel().z, 2) << endl;
    } else {
        reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
		<< "motor / led / accel controls are not currently supported" << endl << endl;
    }

	reportStream << "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
	<< "set near threshold " << near_threshold_ << " (press: + -)" << endl
	<< "set far threshold " << far_threshold_ << " (press: < >) num blobs found " << contour_finder_.nBlobs
	<< ", fps: " << ofGetFrameRate() << endl
	<< "press c to close the connection and o to open it again, connection is: " << kinect_.isConnected() << endl;

    if(kinect_.hasCamTiltControl()) {
    	reportStream << "press UP and DOWN to change the tilt angle: " << tilt_angle_ << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }

	ofDrawBitmapString(reportStream.str(), 20, 652);
  */

	for (size_t i = 0; i < circles_.size(); i++)
  {
		ofFill();
		circles_[i]->draw();
	}

  ofPushStyle();
  ofFill();
  ofSetLineWidth(5.0);
  ofSetHexColor(0xcccccc);
	for (size_t i = 0; i < edge_lines_.size(); i++)
  {
		edge_lines_[i]->draw();
	}
  ofPopStyle();

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

		case 'o':
			kinect_.setCameraTiltAngle(tilt_angle_); // go back to prev tilt
			kinect_.open();
			break;

		case ' ':
      ofToggleFullscreen();
			break;

		case 'r':
      radius_max_ -= 5;
      if (radius_max_ < 5) radius_max_ = 5;
      break;

		case 'R':
      radius_max_ += 5;
      break;

		case 'c':
			kinect_.setCameraTiltAngle(0); // zero the tilt
			kinect_.close();
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
			kinect_.setCameraTiltAngle(tilt_angle_);
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}

void ofApp::exit() {
  kinect_.setCameraTiltAngle(0); // zero the tilt on exit
  kinect_.close();
}


