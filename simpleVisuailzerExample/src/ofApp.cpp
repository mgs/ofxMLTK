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
  int largeFrameSize = 32768;
  int numberOfBuffers = 4;
  int hopSize = 256;
  int largeHopSize = 32768;
  
  soundStream.setup(numberOfOutputChannels, numberOfInputChannels, sampleRate, frameSize, numberOfBuffers);
  mltk.setup(frameSize, sampleRate, hopSize, largeFrameSize, largeHopSize);
  mltk.run();
}

//--------------------------------------------------------------
void ofApp::update(){
   mltk.run();
}

//--------------------------------------------------------------
void ofApp::draw(){
  const vector<Real> &hpcp = mltk.pool.value<vector<vector<Real>>>("hpcp")[0];
  
  ofFill();
  ofSetColor(0,0,0);
  
  for(int i = 0; i < 13; i++){
    ofDrawRectangle(i*(ofGetWidth()/13), 0, 100, abs(hpcp[i])*10000000);
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
