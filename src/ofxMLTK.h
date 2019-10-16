/*
 * Copyright (C) 2019 Michael Simpson [https://mgs.nyc/]
 *
 * ofxMLTK is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 *
 * ---------------------------------------------------------------
 *
 * This project uses Essentia, copyrighted by Music Technology Group - Universitat Pompeu Fabra
 * using GNU Affero General Public License.
 * See http://essentia.upf.edu for documentation.
 *
 */

#ifndef ofxMLTK_h
#define ofxMLTK_h

#pragma once
//#include <ctime>
//using namespace std;
//#include "ofMain.h"

#include <cmath>
#include <functional>
#include <iostream>
#include <map>

#include "algorithmfactory.h"
#include "essentiamath.h"
#include "pool.h"
#include "streaming/streamingalgorithm.h"
#include "streaming/algorithms/vectorinput.h"
#include "streaming/algorithms/vectoroutput.h"
#include "streaming/algorithms/ringbufferinput.h"
#include "streaming/algorithms/ringbufferoutput.h"
#include "streaming/algorithms/poolstorage.h"
#include "streaming/accumulatoralgorithm.h"
#include "scheduler/network.h"

using namespace std;
using namespace chrono;
using namespace essentia;
using namespace streaming;
using namespace scheduler;

//class baseMLTK {
//public:
//  virtual void algorithms(bool useThisInsteadOfDefault) = 0;
//  virtual void chain(bool useThisInsteadOfDefault) = 0;
//};

class MLTK {
public:
  // This boolean is used to toggle the recording of data to an output
  // file in the YAML format. YAML is a data format similar to JSON
  bool recording = false;
  
  
  
  // This boolean controls whether the pool should accumulate values or
  // be cleared on each frame
  bool accumulating = false;
  
  // NOT CURRENTLY IMPLEMENTED
  //  // !!!IMPORTANT!!! To setup your own Algorithm stream set customMode to true
  //  // and implement setupCustomAlgorithms)() and connectUserAlgorithmStream().
  //  // setupAlgorithms() and connectAlgorithmStream() can be used as references.
  //  //  bool customMode = false;
  // NOT CURRENTLY IMPLEMENTED
  
  // These soundbuffers contain the data coming in from openFrameworks
  ofSoundBuffer leftAudioBuffer, rightAudioBuffer;
//  vector<Real> leftAudioBuffer, rightAudioBuffer;
  
  // Vector holding the individuals channels
  vector<ofSoundBuffer> channels;
  
  // Not currently being used
  // std::map<std::string, VectorInput<Real>> inputMap;
  VectorInput<Real> *inputVec, *leftInputVec, *rightInputVec;
  
  VectorInput<Real> *inputX;
  
  // a vector for handling input containing complex values
  //  VectorInput<std::complex<Real>> *complexInput;
  //  VectorOutput<std::vector<std::complex<Real>>> *complexOutput;

  // Ring buffer provided by essentia
  //  essentia::streaming::RingBufferInput *ringIn;
  //  essentia::streaming::RingBufferOutput *ringOut;
  
  // Pointer to the algorithm network
  scheduler::Network *network=NULL;
  
  // Pool objects for collecting, aggregating, and holding statistics.
  Pool pool, poolAggr, poolStats;

  // Dispatch Table, planned for future
  //  std::map<string, function<vector<Real>()>> db;
  map<string, Algorithm*> algorithms;
  
  string fileName;
  
  int numberOfOutputChannels = 0;
  int numberOfInputChannels = 2;
  int sampleRate = 44100;
  int frameSize = 2048;
  int hopSize = frameSize/2;
  int numberOfBuffers = 4;

  essentia::standard::Algorithm *aggr, *output;
  
  std::vector<Real> audioBuffer;

  std::vector<Real> smoothInput;
  
  int binsPerOctave = 12;
  
  template <class mType>
  bool exists(string algorithm);
  
  Real getValue(string algorithm);
  Real getMeanValue(string algorithm);
  vector<Real> getData(string algorithm);
  vector<Real> getMeanData(string algorithm);
  vector<vector<Real>> getRaw(string algorithm);

  void drawGraph(string algorithm, int x, int y, int w, int h);
  void setup(int frameSize, int sampleRate, int hopSize, bool useDefaultAlgorithms);
  void setup(ofSoundStream s, bool useDefaultAlgorithms);

  // Loads most of Essentia's streaming algorithms into the algorithm registry
  void setupAlgorithms(essentia::streaming::AlgorithmFactory& factory);

  // Connects a user-defined algorithm chain
  void connectAlgorithmStream(essentia::streaming::AlgorithmFactory& factory);

  // Connects a default algorithm chain
  void connectDefaultAlgorithmStream(essentia::streaming::AlgorithmFactory& factory);

  template <typename... Params>
  void create(map<string, Algorithm*> &m, essentia::streaming::AlgorithmFactory& f, string algo, Params... params);

  map<string, float[2]> minMaxMap;

  void update();
  void run();
  void save();
  
  void exit();
};

#endif /* MLTK_h */
