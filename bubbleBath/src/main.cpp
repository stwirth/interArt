#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main(int argc, char **argv ){

  ofWindowMode screen_mode = OF_WINDOW;

  for (int i = 1; i < argc; ++i)
  {
    if (std::string(argv[i]) == std::string("-f"))
    {
      screen_mode = OF_FULLSCREEN;
    }
  }

  const int width = 2048;
  const int height = 1536;
	ofSetupOpenGL(width, height, screen_mode);			// <-------- setup the GL context

	ofRunApp(new ofApp());

  return EXIT_SUCCESS;

}
