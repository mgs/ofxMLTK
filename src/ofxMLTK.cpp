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

#include "ofxMLTK.h"

void MLTK::setup(int frameSize, int sampleRate, int hopSize, int largeFrameSize, int largeHopSize){
  this->frameSize = frameSize;
  audioBuffer.resize(frameSize*2, 0.0);
  leftAudioBuffer.resize(frameSize, 0.0);
  rightAudioBuffer.resize(frameSize, 0.0);
  
  essentia::init();
  essentia::streaming::AlgorithmFactory& factory = essentia::streaming::AlgorithmFactory::instance();
  factory.init();
  
  audioBuffer.resize(frameSize, 0.0);
  leftAudioBuffer.resize(frameSize, 0.0);
  rightAudioBuffer.resize(frameSize, 0.0);
  inputVec = new VectorInput<Real>(&audioBuffer);
  inputVec->setVector(&audioBuffer);

  dcremoval = factory.create("DCRemoval",
                              "sampleRate", sampleRate);

  fc    = factory.create("FrameCutter",
                          "frameSize",frameSize,
                          "hopSize", hopSize,
                          "startFromZero" , true,
                          "validFrameThresholdRatio", 0,
                          "lastFrameToEndOfFile", true);
  
  fc2    = factory.create("FrameCutter",
                           "frameSize",largeFrameSize,
                           "hopSize", largeHopSize,
                           "startFromZero" , true,
                           "validFrameThresholdRatio", 0,
                           "lastFrameToEndOfFile", true);
  
  
  spectralPeaks = factory.create("SpectralPeaks",
                                  "maxPeaks", PEAKS_MAXPEAKS_NUM,
                                  "minFrequency", PEAKS_MIN_FREQ,
                                  "maxFrequency", PEAKS_MAX_FREQ);
  
  w = factory.create("Windowing",
                      "type", windowType,
                      "zeroPhase", true,
                      "zeroPadding", 0);
  
  w2 = factory.create("Windowing",
                       "type", windowType,
                       "zeroPhase", true,
                       "zeroPadding", 0);
  
  fft  = factory.create("FFT");
    //  fft2  = factory.create("FFT");
  
  spec  = factory.create("Spectrum",
                          "size", (frameSize/2));
  
  bfcc  = factory.create("BFCC",
                          "inputSize", (frameSize/2)+1);
  
  mfcc  = factory.create("MFCC",
                          "dctType", 2,
                          "inputSize", (frameSize/2)+1);
  
  gfcc  = factory.create("GFCC",
                          "inputSize", (frameSize/2)+1);
  
  lpc = factory.create("LPC",
                        "order", LPC_COEFS,
                        "sampleRate", sampleRate,
                        "type", "regular");
  
  rms = factory.create("RMS");
  energy = factory.create("Energy");
  power = factory.create("InstantPower");
  
  centroid = factory.create("Centroid", "range", sampleRate/2);
  
  spectralPeaks = factory.create("SpectralPeaks",
                                  "maxPeaks", PEAKS_MAXPEAKS_NUM,
                                  "magnitudeThreshold", 0.00001,
                                  "minFrequency", PEAKS_MIN_FREQ,
                                  "maxFrequency", PEAKS_MAX_FREQ,
                                  "orderBy", "frequency");
  
  
  pitchDetect = factory.create("PitchYinFFT",
                                "frameSize", frameSize,
                                "sampleRate", sampleRate);
  
  hpcp = factory.create("HPCP",
                         "size", HPCP_SIZE,
                         "referenceFrequency", 440,
                         "bandPreset", false,
                         "minFrequency", HPCP_MIN_FREQ,
                         "maxFrequency", HPCP_MAX_FREQ,
                         "weightType", "squaredCosine",
                         "nonLinear", false,
                         "windowSize", 4.0/3.0);
  
  c2p = factory.create("CartesianToPolar");
  
  chromagram = factory.create("Chromagram",
                               "binsPerOctave", binsPerOctave);
  
  std::cout << "-------- connecting algorithm stream --------" << std::endl;
  
  *inputVec >> dcremoval->input("signal");  
  dcremoval->output("signal") >> fc->input("signal");  
  fc->output("frame") >> w->input("frame");  
  w->output("frame") >> fft->input("frame");    
  w->output("frame") >> rms->input("array");  

  fft->output("fft") >> c2p->input("complex");
  c2p->output("magnitude") >> PC(pool, "magnitudes");
  c2p->output("phase") >> PC(pool, "phases");
  
  *inputVec >> fc2->input("signal");
  fc2->output("frame") >> lpc->input("frame");
  fc2->output("frame") >> PC(pool, "frame2");
  w->output("frame") >> spec->input("frame");
  spec->output("spectrum")  >>  gfcc->input("spectrum");
  spec->output("spectrum")  >>  bfcc->input("spectrum");
  spec->output("spectrum")  >> mfcc->input("spectrum");
  spec->output("spectrum")  >>  spectralPeaks->input("spectrum");
  spectralPeaks->output("frequencies") >> hpcp->input("frequencies");
  spectralPeaks->output("magnitudes") >> hpcp->input("magnitudes");
  
  // Pool Outputs
  dcremoval->output("signal") >> PC(pool, "dcremoval");
  fc->output("frame") >> PC(pool, "frame");
  w->output("frame") >> PC(pool, "window");
  rms->output("rms") >> PC(pool, "rms");
  spec->output("spectrum")  >>  PC(pool, "spectrum");
  
  bfcc->output("bands")  >>  PC(pool, "bfcc.bands");
  bfcc->output("bfcc")  >>  PC(pool, "bfcc.coefs");
  
  gfcc->output("bands")  >>  PC(pool, "gfcc.bands");
  gfcc->output("gfcc")  >>  PC(pool, "gfcc.coefs");
  
  mfcc->output("bands") >> PC(pool, "mfcc.bands");
  mfcc->output("mfcc") >> PC(pool, "mfcc.coefs");
  
  hpcp->output("hpcp") >> PC(pool, "hpcp");

  lpc->output("lpc") >> PC(pool, "lpc.coefs");
  lpc->output("reflection") >> PC(pool, "lpc.reflection");
  
  factory.shutdown();
  
  network = new scheduler::Network(inputVec);
  network->run();  
}

void MLTK::run(){
  pool.clear();
  network->reset();
  network->run();
}

void MLTK::exit(){
  if(network != NULL){
    network->clear();
  }
  pool.clear();
  essentia::shutdown();
    
  delete network, inputVec;
}
