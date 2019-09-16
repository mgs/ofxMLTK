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

class MLTK {
public:
  int frameSize;
  bool recording = false;
  
  ofSoundBuffer leftAudioBuffer, rightAudioBuffer, tmpLeftBuffer, tmpRightBuffer;
  
  std::map<std::string, VectorInput<Real>> inputMap;
  VectorInput<Real> *inputVec, *leftInputVec, *rightInputVec;
  VectorInput<std::complex<Real>> *complexOut;
  VectorOutput<std::vector<std::complex<Real>>> *outputVec;
  
  essentia::streaming::RingBufferInput *ringIn;
  essentia::streaming::RingBufferOutput *ringOut;
  
  scheduler::Network *network=NULL;
  
  Pool pool, poolAggr, poolStats;
  
  std::map<string, function<vector<Real>()>> db;
  
  map<string, Algorithm*> algoDB;
  
  void setupAlgorithms(essentia::streaming::AlgorithmFactory& factory, int frameSize, int sampleRate, int hopSize, int largeFrameSize, int largeHopSize);
  void connectAlgorithmStream(essentia::streaming::AlgorithmFactory& factory);
  
  Algorithm *rms, *energy, *power, *pitchSalience,
  *pitchSalienceFunction, *pitchSalienceFunctionPeaks,
  *inharmonicity, *hfc, *centroid, *spectralComplexity,
  *dissonance, *rollOff, *oddToEven, *strongPeak, *strongDecay,
  *onsetHfc, *onsetComplex, *onsetFlux, *pitchDetect, *env,
  *dct, *melBands, *fft, *fft2, *ifft, *fftc, *ifftc, *harmonicPeaks,
  *chromagram, *chromaprinter, *spectrumcq, *bandPass, *superFlux,
  *tristimulus, *dcremoval, *barkExtractor, *spec, *fc, *fc2, *p2c,
  *c2p, *w, *w2, *cq, *frameToReal, *hpcp, *lpc, *spectralPeaks, *overlapAdd,
  *resampleFFT, *rhythmTransform, *poolAgg, *aggregator,
  *realAcc, *spectrum,*triF,*superFluxF,*centroidF,*mfccF,*pspectrum, *gfcc;
  
//  streaming::AlgorithmFactory *factory;
  
  essentia::streaming::Algorithm *loudness,*flatness, *cent, *yin, *mfcc, *bfcc,
                                 *TCent, *superFluxP;
  
  essentia::standard::Algorithm *aggr, *output;
  
    //  Algorithm *mag;
    //  Algorithm *constantq;
  
    // Vector of Values
    //  Algorithm *constantq;
    //  Algorithm *frameCutterC;
  
  std::vector<Real> audioBuffer;

  std::string windowType = "hamming";
  int binsPerOctave = 12;
  
  void setup(int frameSize, int sampleRate, int hopSize, int largeFrameSize, int largeHopSize);
  void setup(int frameSize, int sampleRate, int hopSize);

//  mType get(string algorithm);

//  Real get(string algorithm);
//  vector<Real> get(string algorithm);
//  vector<vector<Real>> get(string algorithm);

  template <class mType>
  bool pool_contains(string algorithm);
  
//  template<class mType>
  Real get_real(string algorithm);
  vector<Real> get_vector(string algorithm);
  vector<vector<Real>> get_full_vector(string algorithm);
  
  void update();
  void run();
  void save();
  
  void exit();
};


#endif /* MLTK_h */
