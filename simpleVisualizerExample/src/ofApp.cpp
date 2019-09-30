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
  rms = mltk.getValue("RMS");
  spectrum = mltk.getData("Spectrum");
  
   mltk.run();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofFill();
  ofSetColor(0,0,0);
  
  for(int i = 0; i < spectrum.size(); i++){
    ofDrawRectangle(i*(ofGetWidth()/spectrum.size()), ofGetHeight()/2, ofGetWidth()/spectrum.size(), abs(spectrum[i])*ofGetHeight());
  }
  
}

void ofApp::audioIn(ofSoundBuffer &inBuffer){
  inBuffer.getChannel(mltk.leftAudioBuffer, 0);
  inBuffer.getChannel(mltk.rightAudioBuffer, 1);
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
