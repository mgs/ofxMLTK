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

#include "ofMain.h"
#include "ofxMLTK.h"

void MLTK::setupAlgorithms(essentia::streaming::AlgorithmFactory& factory){
    // Using Essentia's VectorInput type and pointing it at the audioBuffer reference
  inputVec = new VectorInput<Real>(&audioBuffer);
  inputVec->setVector(&audioBuffer);
  
    // if a file is passed, load it into one of essentia's MonoLoader objects
    // which creates a mono data stream, demuxing stereo if needed.
  if(fileName.length() > 0){
    algorithms["MonoLoader"] = factory.create("MonoLoader",
                                              "filename", fileName,
                                              "sampleRate", sampleRate);
  }
  algorithms = {
    { "FrameCutter", factory.create("FrameCutter",
                                    "frameSize", frameSize,
                                    "hopSize", hopSize,
                                    "startFromZero", true,
                                    "validFrameThresholdRatio", 0,
                                    "lastFrameToEndOfFile", true) },
    
    { "Windowing", factory.create("Windowing",
                                  "type", "hann") },
    
    { "DCRemoval", factory.create("DCRemoval",
                                  "sampleRate", sampleRate) },
    
    { "Spectrum", factory.create("Spectrum") },
    
    { "SpectralPeaks", factory.create("SpectralPeaks")},
    
    { "RMS",  factory.create("RMS") },
    
    { "HPCP", factory.create("HPCP") },
    
    { "PoolAggregator", factory.create("PoolAggregator")},
    
    { "BeatsLoudness", factory.create("BeatsLoudness") },
    
    { "Beatogram", factory.create("Beatogram") },
    
    { "Energy",  factory.create("Energy") },
    
    { "InstantPower",  factory.create("InstantPower") },
    
    { "Centroid",  factory.create("Centroid", "range", sampleRate/2) },
    
    { "MFCC", factory.create("MFCC",
                             "normalize", "unit_sum") },
  };
}

void MLTK::connectAlgorithmStream(essentia::streaming::AlgorithmFactory& factory){
  std::cout << "-------- connecting algorithm stream --------" << std::endl;
  
  // We start with the incoming signal that was attached to inputVec
  *inputVec >> algorithms["DCRemoval"]->input("signal");
  
  // Remember that all the strings match 1:1 with Essentia's reference documentation.
  // Algorithms can have an unlimited number of OUTPUTS but every input must
  // always have exactly 1 connection (inputs always need to be connected)
  algorithms["DCRemoval"]->output("signal") >> algorithms["FrameCutter"]->input("signal");
  algorithms["FrameCutter"]->output("frame") >> algorithms["Windowing"]->input("frame");
  algorithms["Windowing"]->output("frame") >> algorithms["RMS"]->input("array");
  algorithms["Windowing"]->output("frame") >> algorithms["Spectrum"]->input("frame");
  algorithms["Spectrum"]->output("spectrum")  >> algorithms["MFCC"]->input("spectrum");
  algorithms["Spectrum"]->output("spectrum") >> algorithms["SpectralPeaks"]->input("spectrum");
  algorithms["SpectralPeaks"]->output("frequencies") >> algorithms["HPCP"]->input("frequencies");
  algorithms["SpectralPeaks"]->output("magnitudes") >> algorithms["HPCP"]->input("magnitudes");
  
    // Pool Outputs
  algorithms["DCRemoval"]->output("signal") >> PC(pool, "DCRemoval");
  algorithms["FrameCutter"]->output("frame") >> PC(pool, "FrameCutter");
  algorithms["Windowing"]->output("frame") >> PC(pool, "Windowing");
  algorithms["RMS"]->output("rms") >> PC(pool, "RMS");
  algorithms["Spectrum"]->output("spectrum")  >>  PC(pool, "Spectrum");
  algorithms["MFCC"]->output("mfcc") >> PC(pool, "MFCC.coefs");
  algorithms["MFCC"]->output("bands") >> PC(pool, "MFCC.bands");
  algorithms["HPCP"]->output("hpcp") >> PC(pool, "HPCP");
  
  
  factory.shutdown();
}

void MLTK::setup(int frameSize=1024, int sampleRate=44100, int hopSize=512){
  this->frameSize = frameSize;
  this->sampleRate = frameSize;
  this->hopSize = frameSize;
  
  essentia::init();
  
  essentia::streaming::AlgorithmFactory& factory = essentia::streaming::AlgorithmFactory::instance();
  
  factory.init();
  
  audioBuffer.resize(frameSize, 0.0);
  leftAudioBuffer.getBuffer().resize(frameSize, 0.0);
  rightAudioBuffer.getBuffer().resize(frameSize, 0.0);
  
  setupAlgorithms(factory);
  connectAlgorithmStream(factory);
  
  network = new scheduler::Network(inputVec);
  network->run();
}

void MLTK::run(){
  update();
  
  pool.clear();
  network->reset();
  network->run();
  
  if(recording){
    aggr->input("input").set(pool);
    aggr->output("output").set(poolAggr);
    
    aggr->compute();
  }
}

template <class mType>
bool MLTK::exists(string algorithm){
  return pool.contains<mType>(algorithm);
};

vector<Real> MLTK::getData(string algorithm){
//  if(exists<vector<Real>>(algorithm)){
    return pool.value<vector<vector<Real>>>(algorithm)[0];
//  } else {
//    cout << "No data in pool for: " + algorithm << ". Is the spelling good?";
//  }
};

vector<vector<Real>> MLTK::getRaw(string algorithm){
  return pool.value<vector<vector<Real>>>(algorithm);
};

Real MLTK::getValue(string algorithm){
  Real val = pool.value<vector<Real>>(algorithm)[0];
  return val;
};

void MLTK::update(){
    //  {
    //    std::lock_guard<std::mutex>mtx(mutex);
    //    leftAudioBuffer = tmpLeftBuffer;
    //    rightAudioBuffer = tmpRightBuffer;
  for (int i = 0; i < frameSize; i++){
    audioBuffer[i] = (Real) ((leftAudioBuffer[i]) + (rightAudioBuffer[i])) / 2;
  }
    //  }
}

void MLTK::save(){
  output->input("pool").set(poolAggr);
  output->compute();
  
    //  pool.clear();
    //  network->reset();
    //  network->run();
}

void MLTK::exit(){
  if(network != NULL){
    network->clear();
  }
  pool.clear();
  poolAggr.clear();
  essentia::shutdown();
  
  delete aggr, output, network, inputVec;
}
