#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
  // Setup the Window
  //
  //  ofClear(backgroundColor.get());;
  ofSetVerticalSync(true);
  ofSetBackgroundAuto(true);
  ofSetWindowTitle("MLTK DEMO");

  int numberOfOutputChannels = 0;
  int numberOfInputChannels = 2;
  int sampleRate = 44100;
  int frameSize = 512;
  int hopSize = 256;
  int numberOfBuffers = 4;
  
  soundStream.setup(numberOfOutputChannels, numberOfInputChannels, sampleRate, frameSize, numberOfBuffers);
  mltk.setup(frameSize, sampleRate, hopSize);
  mltk.run();
}

//--------------------------------------------------------------
void ofApp::update(){
   mltk.run();
}

//--------------------------------------------------------------
void ofApp::draw(){
  const Real &rms = mltk.getValue("RMS");
  const vector<Real> &spec = mltk.getData("Spectrum");
  
  ofFill();
  ofSetColor(0,0,0);
  
  for(int i = 0; i < spec.size(); i++){
    ofDrawRectangle(i*(ofGetWidth()/spec.size()), ofGetHeight()/2, ofGetWidth()/spec.size(), abs(spec[i])*ofGetHeight());
  }
  
}

void ofApp::audioIn(ofSoundBuffer &inBuffer){
  int bufCounter = 0;
  
  for (int i = 0; i < inBuffer.getBuffer().size(); i += 2){
    mltk.leftAudioBuffer[bufCounter] = (Real) (inBuffer.getBuffer()[i]);
    mltk.rightAudioBuffer[bufCounter] = (Real) (inBuffer.getBuffer()[i+1]);
    
    mltk.audioBuffer[bufCounter] = (Real) ((inBuffer.getBuffer()[i]) + (inBuffer.getBuffer()[i+1])) / 2;
    bufCounter++;
  }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
