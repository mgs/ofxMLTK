/*
 * Copyright (C) 2019 Michael Simpson [http://mgs.nyc/]
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

  //----------------------------------
  //for scaling values:
#define DB_MIN -6
#define DB_MAX 0
#define MFCC_MAX_ESTIMATED_VALUE 300.0
  //----------------------------------
  //Vars for algorithm creation:
#define MELBANDS_BANDS_NUM 256
#define DCT_COEFF_NUM 13
#define PITCH_SALIENCE_FUNC_NUM 10
#define TRISTIMULUS_BANDS_NUM 3

#define HPCP_SIZE 12
#define HPCP_MIN_FREQ 40.0//hz
#define HPCP_MAX_FREQ 5000.0//hz

#define PEAKS_MAXPEAKS_NUM 10000
#define PEAKS_MIN_FREQ 40.0//hz
#define PEAKS_MAX_FREQ 5000.0//hz

#define LPC_COEFS 70
#define LPC_MIN -1.0
#define LPC_MAX 1.0
  //----------------------------------
  // multiply novelty function and threshold for easier parameterization,
  // the true computation is Not Multiplied, only inputs and outputs
#define NOVELTY_MULT 1

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
  int frameSize;
  bool recording = false;
  
  // !!!IMPORTANT!!! To setup your own Algorithm stream set customMode to true
  // and implement setupCustomAlgorithms)() and connectUserAlgorithmStream().
  // setupAlgorithms() and connectAlgorithmStream() can be used as references.
//  bool customMode = false;

  // These soundbuffers contain the data coming in from openFrameworks
  ofSoundBuffer leftAudioBuffer, rightAudioBuffer;
  
  // Not currently being used
  // std::map<std::string, VectorInput<Real>> inputMap;
  VectorInput<Real> *inputVec, *leftInputVec, *rightInputVec;
  
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
//  map<string, Algorithm*> algoDB;
  
  // This is a list of pointers for essentia::algorithms::streaming objects
  // this should be refactored but currently any algorithm created by the
  // factory needs to be associated with a pointer here. The algoDB above will
  // likely replace this in the future but it's a slightly more verbose solution.
  Algorithm *monoLoader, *rms, *energy, *power, *pitchSalience,
  *pitchSalienceFunction, *pitchSalienceFunctionPeaks,
  *inharmonicity, *hfc, *centroid, *spectralComplexity,
  *dissonance, *rollOff, *oddToEven, *strongPeak, *strongDecay,
  *onsetHfc, *onsetComplex, *onsetFlux, *pitchDetect, *env,
  *dct, *melBands, *fft, *fft2, *ifft, *fftc, *ifftc, *harmonicPeaks,
  *chromagram, *chromaprinter, *spectrumcq, *bandPass, *superFlux,
  *tristimulus, *dcremoval, *barkExtractor, *spec, *fc, *fc2, *p2c,
  *c2p, *w, *w2, *cq, *frameToReal, *hpcp, *lpc, *spectralPeaks, *overlapAdd,
  *resampleFFT, *rhythmTransform, *poolAgg, *aggregator,
  *realAcc, *spectrum,*triF,*superFluxF,*centroidF,*mfccF,*pspectrum, *gfcc,
  *loudness,*flatness, *cent, *yin, *mfcc, *bfcc, *TCent, *superFluxP;
  
  essentia::standard::Algorithm *aggr, *output;
  
  std::vector<Real> audioBuffer;

  std::string windowType = "hann";
  int binsPerOctave = 12;
  
  template <class mType>
  bool pool_contains(string algorithm);
  
  Real get_real(string algorithm);
  vector<Real> get_vector(string algorithm);
  vector<vector<Real>> get_full_vector(string algorithm);
  
  void setup(int frameSize, int sampleRate, int hopSize, int largeFrameSize, int largeHopSize);
  void setup(int frameSize, int sampleRate, int hopSize, string fileName);
  
  void setupAlgorithms(essentia::streaming::AlgorithmFactory& factory, int frameSize, int sampleRate, int hopSize, int largeFrameSize, int largeHopSize, string fileName);
  void connectAlgorithmStream(essentia::streaming::AlgorithmFactory& factory);
  
    // this method contains the step where essentia algorithms are created by
    // the "factory"
//  void setupCustomAlgorithms();
  
    // this method is where we patch the inputs and outputs of the algorithms
    // objects we created in the previous method.
//  void connectCustomAlgorithmStream();
  
  void update();
  void run();
  void save();
  
  void exit();
};

#endif /* MLTK_h */
