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

class MLTK {
public:
  MLTK() {
    channelSoundBuffers.assign(numberOfInputChannels, ofSoundBuffer());
    channelAudioBuffers.assign(numberOfInputChannels, vector<Real>());
    chPools.assign(numberOfInputChannels, Pool());
    chPoolAggrs.assign(numberOfInputChannels, Pool());
    chPoolStats.assign(numberOfInputChannels, Pool());
    chAlgorithms.assign(numberOfInputChannels, map<string, essentia::streaming::Algorithm*>());
  }

  // This boolean is used to toggle the recording of data to an output
  // file in the YAML format. YAML is a data format similar to JSON
  bool recording = false;

  // This boolean controls whether the pool should accumulate values or
  // be cleared on each frame
  bool accumulating = false;

  // These soundbuffers contain the data coming in from openFrameworks
  vector<ofSoundBuffer> channelSoundBuffers;

  VectorInput<Real> *monoInputVec = NULL;
  vector<VectorInput<Real>*> channelInputVectors;

  VectorInput<Real> *inputX;

  // a vector for handling input containing complex values
  //  VectorInput<std::complex<Real>> *complexInput;
  //  VectorOutput<std::vector<std::complex<Real>>> *complexOutput;

  // Ring buffer provided by essentia
  //  essentia::streaming::RingBufferInput *ringIn;
  //  essentia::streaming::RingBufferOutput *ringOut;

  // Pointer to the algorithm network
  scheduler::Network *monoNetwork=NULL;
  vector<scheduler::Network*> chNetworks;

  // Pool objects for collecting, aggregating, and holding statistics.
  Pool monoPool, monoPoolAggr, monoPoolStats;
  vector<Pool> chPools;
  vector<Pool> chPoolAggrs;
  vector<Pool> chPoolStats;

  // Dispatch Table, planned for future
//  std::map<string, function<vector<Real>()>> db;
  map<string, essentia::streaming::Algorithm*> monoAlgorithms;
  vector<map<string, essentia::streaming::Algorithm*>> chAlgorithms;

  string fileName;

  int numberOfOutputChannels = 0;
  // This should match the number of input channels in your input
  int numberOfInputChannels = 1;
  // the sampleRate should match the rate of of your sound card, you can check
  // this "Audio MIDI Setup.app" found in the Utilities folder of Applications
  int sampleRate = 44100;
  int frameSize = 1024;
  int hopSize = frameSize/2;
  int numberOfBuffers = 1;

  essentia::standard::Algorithm *aggr, *output;
  vector<essentia::standard::Algorithm*> chAggr, chOutput;

  vector<Real> monoAudioBuffer;
  vector<vector<Real>> channelAudioBuffers;

  vector<Real> smoothInput;

  int binsPerOctave = 12;
  
  template <class mType>
  bool exists(string algorithm, int channel = -1);
  
  // -1 = mono aggregate channel
  Real getValue(string algorithm, int channel = -1);
  vector<Real> getData(string algorithm, int channel = -1);
  vector<vector<Real>> getRaw(string algorithm, int channel = -1);
  
  void setup(int frameSize, int sampleRate, int hopSize);

  template <typename... Params>
  void create(map<string, Algorithm*> &m, essentia::streaming::AlgorithmFactory& f, string algo, Params... params);

  void setupAlgorithms(essentia::streaming::AlgorithmFactory& factory,
                       VectorInput<Real>* inputVec,
                       vector<Real> audioBuffer,
                       map<string, Algorithm*>& algorithms);

  void connectAlgorithmStream(VectorInput<Real>* inputVec,
                              map<string, Algorithm*>& algorithms,
                              Pool& pool);

  void update();
  void run();
  void save();
  void exit();
};

#endif /* MLTK_h */
