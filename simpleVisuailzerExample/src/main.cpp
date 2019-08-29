#include "ofMain.h"
#include "ofAppGLFWWindow.h"
#include "ofApp.h"

  //========================================================================
int main( ){
  ofGLFWWindowSettings settings;
    //this is an ugly hack only for osx in order to get back the whole depth buffer.
    //It is an openGL driver issue....
  settings.numSamples = 0;
  settings.setGLVersion(3, 2);
  settings.multiMonitorFullScreen = false;
  
  settings.resizable = false;
  settings.setSize(1080,1080);
  settings.windowMode = OF_WINDOW;
  ofCreateWindow(settings);
  ofRunApp(new ofApp());
}
