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

template <typename... Params>
void MLTK::create(map<string, Algorithm*> &m, essentia::streaming::AlgorithmFactory& f, string algo, Params... params){
  m[algo] =  f.create(algo);
};

void MLTK::setupAlgorithms(essentia::streaming::AlgorithmFactory& f,
                           VectorInput<Real>* inputVec,
                           vector<Real> audioBuffer,
                           map<string, Algorithm*>& algorithms) {

  // Using Essentia's VectorInput type and pointing it at the audioBuffer reference
  inputVec = new VectorInput<Real>(&audioBuffer);
  inputVec->setVector(&monoAudioBuffer);

  map<string, essentia::streaming::Algorithm*> newAlgorithms = {
      // Envelope/SFX category
      //
      // afterMaxToBeforeMaxEnergyRatio
      // input: pitch (real) - the array of pitch values [Hz]
      // output: afterMaxToBeforeMaxEnergyRatio (real) - the ratio between the pitch energy after the pitch maximum to the pitch energy before the pitch maximum
      // description: This algorithm computes the ratio between the pitch energy after the pitch maximum and the pitch energy before the pitch maximum. Sounds having an monotonically ascending pitch or one unique pitch will show a value of (0,1], while sounds having a monotonically descending pitch will show a value of [1,∞). In case there is no energy before the max pitch, the algorithm will return the energy after the maximum pitch.
      //
      //The algorithm throws exception when input is either empty or contains only zeros.
      { "AfterMaxToBeforeMaxEnergyRatio", f.create("AfterMaxToBeforeMaxEnergyRatio") },
      
      // DerivativeSFX
      // inputs: envelope (vector_real) - the envelope of the signal
      // output: derAvAfterMax (real) - the weighted average of the derivative after the maximum amplitude
      // output: maxDerBeforeMax (real) - the maximum derivative before the maximum amplitude
      // This algorithm computes two descriptors that are based on the derivative of a signal envelope.
      //
      // The first descriptor is calculated after the maximum value of the input signal occurred. It is the average of the signal's derivative weighted by its amplitude. This coefficient helps discriminating impulsive sounds, which have a steep release phase, from non-impulsive sounds. The smaller the value the more impulsive.
      //
      // The second descriptor is the maximum derivative, before the maximum value of the input signal occurred. This coefficient helps discriminating sounds that have a smooth attack phase, and therefore a smaller value than sounds with a fast attack.
      //
      // This algorithm is meant to be fed by the outputs of the Envelope algorithm. If used in streaming mode, RealAccumulator should be connected in between. An exception is thrown if the input signal is empty.
      //
      // See also: Envelope (streaming) RealAccumulator (streaming)
      { "DerivativeSFX", f.create("DerivativeSFX") },
      
      // Envelope
      // input: signal (real) - the input signal
      // output: signal (real) - the resulting envelope of the signal
      // Params:
      //   applyRectification (bool ∈ {true, false}, default = true) : whether to apply rectification (envelope based on the absolute value of signal)
      //   attackTime (real ∈ [0, ∞), default = 10) : the attack time of the first order lowpass in the attack phase [ms]
      //   releaseTime (real ∈ [0, ∞), default = 1500) : the release time of the first order lowpass in the release phase [ms]
      //   sampleRate (real ∈ (0, ∞), default = 44100) : the audio sampling rate [Hz]
      //
      // This algorithm computes the envelope of a signal by applying a non-symmetric lowpass filter on a signal. By default it rectifies the signal, but that is optional.
      // References:
      // [1] U. Zölzer, Digital Audio Signal Processing, John Wiley & Sons Ltd, 1997, ch.7
      { "Envelope", f.create("Envelope") },
      
      // FlatnessSFX
      // input: envelope (vector_real) - the envelope of the signal
      // output: flatness (real) - the flatness coefficient
      // description: This algorithm calculates the flatness coefficient of a signal envelope.
      //
      // There are two thresholds defined: a lower one at 20% and an upper one at 95%. The thresholds yield two values: one value which has 20% of the total values underneath, and one value which has 95% of the total values underneath. The flatness coefficient is then calculated as the ratio of these two values. This algorithm is meant to be plugged after Envelope algorithm, however in streaming mode a RealAccumulator algorithm should be connected in between the two. In the current form the algorithm can't be calculated in streaming mode, since it would violate the streaming mode policy of having low memory consumption.
      //
      // An exception is thrown if the input envelope is empty.
      //
      // See Also: Envelope (streaming) RealAccumulator (streaming)
      { "FlatnessSFX", f.create("FlatnessSFX") },
      
      // LogAttackTime
      //
      // input: signal (vector_real) - the input signal envelope (must be non-empty)
      //
      // output:
      //   logAttackTime (real) - the log (base 10) of the attack time [log10(s)]
      //   attackStart (real) - the attack start time [s]
      //   attackStop (real) - the attack end time [s]
      //
      // Parameters:
      //   sampleRate (real ∈ (0, ∞), default = 44100) : the audio sampling rate [Hz]
      //   startAttackThreshold (real ∈ [0, 1], default = 0.2) : the percentage of the input signal envelope at which the starting point of the attack is considered
      //   stopAttackThreshold (real ∈ [0, 1], default = 0.9) : the percentage of the input signal envelope at which the ending point of the attack is considered
      //
      // Description:
      //   This algorithm computes the log (base 10) of the attack time of a signal envelope. The attack time is defined as the time duration from when the sound becomes perceptually audible to when it reaches its maximum intensity. By default, the start of the attack is estimated as the point where the signal envelope reaches 20% of its maximum value in order to account for possible noise presence. Also by default, the end of the attack is estimated as as the point where the signal envelope has reached 90% of its maximum value, in order to account for the possibility that the max value occurres after the logAttack, as in trumpet sounds.
      //
      // With this said, LogAttackTime's input is intended to be fed by the output of the Envelope algorithm. In streaming mode, the RealAccumulator algorithm should be connected between Envelope and LogAttackTime.
      //
      // Note that startAttackThreshold cannot be greater than stopAttackThreshold and the input signal should not be empty. In any of these cases an exception will be thrown.
      // See Also: Envelope (streaming) RealAccumulator (streaming)
      { "LogAttackTime", f.create("LogAttackTime") },
      
      
      { "MaxToTotal", f.create("MaxToTotal") },
      
      { "MinToTotal", f.create("MinToTotal") },
      
      { "StrongDecay", f.create("StrongDecay") },
      
      { "TCToTotal", f.create("TCToTotal") },
      
      // Filters
      
      { "AllPass", f.create("AllPass") },
      
      { "BandPass", f.create("BandPass") },
      
      { "BandReject", f.create("BandReject") },
      
      { "DCRemoval", f.create("DCRemoval",
                                    "sampleRate", sampleRate) },
      
      { "EqualLoudness", f.create("EqualLoudness") },
      
      { "HighPass", f.create("HighPass") },
      
      { "IIR", f.create("IIR") },
      
      { "LowPass", f.create("LowPass") },
      
      { "MaxFilter", f.create("MaxFilter") },
      
      { "MedianFilter", f.create("MedianFilter") },
      
      { "MovingAverage", f.create("MovingAverage") },
      
      // Input/output
      
      { "AudioLoader", f.create("AudioLoader") },
      
      { "AudioOnsetsMarker", f.create("AudioOnsetsMarker") },
      
      { "AudioWriter", f.create("AudioWriter") },
      
      { "EasyLoader", f.create("EasyLoader") },

      { "EqloudLoader", f.create("EqloudLoader") },
      
      { "FileOutput", f.create("FileOutput") },
      
      { "MetadataReader", f.create("MetadataReader") },
      
      { "MonoLoader", f.create("MonoLoader") },
      
      { "MonoWriter", f.create("MonoWriter") },

      //    Deprecated? Use data structure VectorInput instead-
      //    { "VectorInput", f.create("VectorInput") },
      //    { "YamlInput", f.create("YamlInput") },
      //    { "YamlOutput", f.create("YamlOutput") },
      
      // Standard Algorithms
      { "AutoCorrelation", f.create("AutoCorrelation") },
      
      { "BPF", f.create("BPF") },
      
      { "BinaryOperator", f.create("BinaryOperator") },
      
      { "BinaryOperatorStream", f.create("BinaryOperatorStream") },
      
      { "Clipper", f.create("Clipper") },
      
      { "ConstantQ", f.create("ConstantQ") },
      
      { "CrossCorrelation", f.create("CrossCorrelation") },
      
      { "CubicSpline", f.create("CubicSpline") },
      
      { "DCT", f.create("DCT") },
      
      { "Derivative", f.create("Derivative") },
      
      { "FFT", f.create("FFT") },
      
      { "FFTC", f.create("FFTC") },
          
          
      //    FrameCutter
      //    streaming mode | Standard category
      //
      //    Inputs
      //    signal (real) - the input audio signal
      //    Outputs
      //    frame (vector_real) - the frames of the audio signal
      //    Parameters
      //      frameSize (integer ∈ [1, ∞), default = 1024) : the size of the frame to cut
      //      hopSize (integer ∈ [1, ∞), default = 512) : the number of samples to jump after a frame is output
      //      lastFrameToEndOfFile (bool ∈ {true, false}, default = false) : whether the beginning of the last frame should reach the end of file. Only applicable if startFromZero is true
      //      silentFrames (string ∈ {drop, keep, noise}, default = noise) : whether to [keep/drop/add noise to] silent frames
      //      startFromZero (bool ∈ {true, false}, default = false) : whether to start the first frame at time 0 (centered at frameSize/2) if true, or -frameSize/2 otherwise (zero-centered)
      //      validFrameThresholdRatio (real ∈ [0, 1], default = 0) : frames smaller than this ratio will be discarded, those larger will be zero-padded to a full frame (i.e. a value of 0 will never discard frames and a value of 1 will only keep frames that are of length 'frameSize')
      //
      //    Description:
      //    This algorithm slices the input buffer into frames. It returns a frame of a constant size and jumps a constant amount of samples forward in the buffer on every compute() call until no more frames can be extracted; empty frame vectors are returned afterwards. Incomplete frames (frames starting before the beginning of the input buffer or going past its end) are zero-padded or dropped according to the "validFrameThresholdRatio" parameter.
      //
      //    The algorithm outputs as many frames as needed to consume all the information contained in the input buffer. Depending on the "startFromZero" parameter:
      //
      //    startFromZero = true: a frame is the last one if its end position is at or beyond the end of the stream. The last frame will be zero-padded if its size is less than "frameSize"
      //    startFromZero = false: a frame is the last one if its center position is at or beyond the end of the stream
      //    In both cases the start time of the last frame is never beyond the end of the stream.
      { "FrameCutter", f.create("FrameCutter",
                                      "frameSize", frameSize,
                                      "hopSize", hopSize) },

      { "LargeFrameCutter", f.create("FrameCutter",
                                      "frameSize", frameSize*32,
                                      "hopSize", hopSize*32) },

  //                                    "startFromZero", true,
  //                                    "validFrameThresholdRatio", 0,
  //                                    "lastFrameToEndOfFile", true) },
  //
      //    Python Only
      //    { "FrameGenerator", f.create("FrameGenerator") },
      
      { "FrameToReal", f.create("FrameToReal") },
      
      { "IDCT", f.create("IDCT") },
      
      { "IFFT", f.create("IFFT") },
      
      { "IFFTC", f.create("IFFTC") },
      
      { "MonoMixer", f.create("MonoMixer") },
      
      { "Multiplexer", f.create("Multiplexer") },
      
      { "NSGConstantQ", f.create("NSGConstantQ") },
      
      { "NSGConstantQStreaming", f.create("NSGConstantQStreaming") },
      
      { "NSGIConstantQ", f.create("NSGIConstantQ") },
      
      { "NoiseAdder", f.create("NoiseAdder") },

      { "OverlapAdd", f.create("OverlapAdd") },
      
      { "PeakDetection", f.create("PeakDetection") },
      
      { "RealAccumulator", f.create("RealAccumulator") },
      
      { "Resample", f.create("Resample") },
      
      { "Scale", f.create("Scale") },
      
      { "Slicer", f.create("Slicer") },
      
      { "Spline", f.create("Spline") },
      
      { "StereoDemuxer", f.create("StereoDemuxer") },
      
      { "StereoMuxer", f.create("StereoMuxer") },
      
      { "StereoTrimmer", f.create("StereoTrimmer") },
          
      { "Trimmer", f.create("Trimmer") },
      
      { "UnaryOperator", f.create("UnaryOperator") },

      { "UnaryOperatorStream", f.create("UnaryOperatorStream") },
      
      { "VectorRealAccumulator", f.create("VectorRealAccumulator") },
      
      { "WarpedAutoCorrelation", f.create("WarpedAutoCorrelation") },
      
      { "Welch", f.create("Welch") },
      
      { "Windowing", f.create("Windowing",
                              "type", "hann") },

      { "LargeWindowing", f.create("Windowing",
                                   "type", "hann") },

      
      { "ZeroCrossingRate", f.create("ZeroCrossingRate") },

      // Spectral
      
      { "BFCC", f.create("BFCC") },
      
      { "BarkBands", f.create("BarkBands") },
      
      { "ERBBands", f.create("ERBBands") },
      
      { "EnergyBand", f.create("EnergyBand") },
      
      { "EnergyBandRatio", f.create("EnergyBandRatio") },
      
      { "FlatnessDB", f.create("FlatnessDB") },
      
      { "Flux", f.create("Flux") },
      
      { "FrequencyBands", f.create("FrequencyBands") },
      
      { "GFCC", f.create("GFCC") },
      
      { "HFC", f.create("HFC") },
      
      { "LPC", f.create("LPC") },
      
      { "MFCC", f.create("MFCC") },
      
      { "MaxMagFreq", f.create("MaxMagFreq") },
      
      { "MelBands", f.create("MelBands") },
      
      { "Panning", f.create("Panning") },
      
      { "PowerSpectrum", f.create("PowerSpectrum") },
      
      { "RollOff", f.create("RollOff") },
      
      { "SpectralCentroidTime", f.create("SpectralCentroidTime") },
      
      { "SpectralComplexity", f.create("SpectralComplexity") },
      
      { "SpectralContrast", f.create("SpectralContrast") },

      { "SpectralPeaks", f.create("SpectralPeaks") },

      { "SpectralWhitening", f.create("SpectralWhitening") },

      { "SpectrumToCent", f.create("SpectrumToCent") },

      { "StrongPeak", f.create("StrongPeak") },

      { "TriangularBands", f.create("TriangularBands") },

      { "TriangularBarkBands", f.create("TriangularBarkBands") },
      
      // Rhythm

      { "BeatTrackerDegara", f.create("BeatTrackerDegara") },

      { "BeatTrackerMultiFeature", f.create("BeatTrackerMultiFeature") },

      { "Beatogram", f.create("Beatogram") },

      { "BeatsLoudness", f.create("BeatsLoudness") },

      { "BpmHistogram", f.create("BpmHistogram") },

      { "BpmHistogramDescriptors", f.create("BpmHistogramDescriptors") },

      { "Danceability", f.create("Danceability") },
      
      { "HarmonicBpm", f.create("HarmonicBpm") },
      
      { "LoopBpmConfidence", f.create("LoopBpmConfidence") },
      
      { "LoopBpmEstimator", f.create("LoopBpmEstimator") },
      
      { "Meter", f.create("Meter") },
      
      { "NoveltyCurve", f.create("NoveltyCurve") },
      
  //    { "NoveltyCurveFixedBpmEstimator", f.create("NoveltyCurveFixedBpmEstimator") },
      
      { "OnsetDetection", f.create("OnsetDetection") },
      
      { "OnsetDetectionGlobal", f.create("OnsetDetectionGlobal") },
      
      { "OnsetRate", f.create("OnsetRate") },
      
      { "Onsets", f.create("Onsets") },
      
      { "PercivalBpmEstimator", f.create("PercivalBpmEstimator") },
      
      { "PercivalEnhanceHarmonics", f.create("PercivalEnhanceHarmonics") },
      
      { "PercivalEvaluatePulseTrains", f.create("PercivalEvaluatePulseTrains") },
      
      { "RhythmDescriptors", f.create("RhythmDescriptors") },
      
      { "RhythmExtractor2013", f.create("RhythmExtractor2013") },
      
      { "RhythmExtractor", f.create("RhythmExtractor") },
      
      { "RhythmTransform", f.create("RhythmTransform") },
      
      { "SuperFluxExtractor", f.create("SuperFluxExtractor") },
      
      { "SuperFluxNovelty", f.create("SuperFluxNovelty") },
      
      { "SuperFluxPeaks", f.create("SuperFluxPeaks") },
      
      { "TempoScaleBands", f.create("TempoScaleBands") },
      
      { "TempoTap", f.create("TempoTap") },
      
      { "TempoTapMaxAgreement", f.create("TempoTapMaxAgreement") },
      
      { "TempoTapTicks", f.create("TempoTapTicks") },
      
      // Math
      { "CartesianToPolar", f.create("CartesianToPolar") },
      
      { "Magnitude", f.create("Magnitude") },
      
      { "PolarToCartesian", f.create("PolarToCartesian") },
      
      // Statistics
      { "CentralMoments", f.create("CentralMoments") },
      
      { "Centroid", f.create("Centroid") },
      
      { "Crest", f.create("Crest") },
      
      { "Decrease", f.create("Decrease") },
      
      { "DistributionShape", f.create("DistributionShape") },
      
      { "Energy", f.create("Energy") },
      
      { "Entropy", f.create("Entropy") },
      
      { "Flatness", f.create("Flatness") },
      
      { "GeometricMean", f.create("GeometricMean") },
      
      { "Histogram", f.create("Histogram") },
      
      { "InstantPower", f.create("InstantPower") },
      
      { "Mean", f.create("Mean") },
      
      { "Median", f.create("Median") },
      
      { "PoolAggregator", f.create("PoolAggregator") },
      
      { "PowerMean", f.create("PowerMean") },
      
      { "RawMoments", f.create("RawMoments") },
      
      { "SingleGaussian", f.create("SingleGaussian") },
      
      { "Variance", f.create("Variance") },
      
      { "Viterbi", f.create("Viterbi") },
      
      // Tonal
      
      { "ChordsDescriptors", f.create("ChordsDescriptors") },
      
      { "ChordsDetection", f.create("ChordsDetection") },
      
  //    { "ChordsDetectionBeats", f.create("ChordsDetectionBeats") },
      
      { "Chromagram", f.create("Chromagram",
                               "binsPerOctave", 12) },
      
      { "Dissonance", f.create("Dissonance") },
      
      { "HighResolutionFeatures", f.create("HighResolutionFeatures") },
      
      { "Inharmonicity", f.create("Inharmonicity") },
      
      { "Key", f.create("Key") },
      
      { "KeyExtractor", f.create("KeyExtractor") },

      { "NNLSChroma", f.create("NNLSChroma") },
      
      { "OddToEvenHarmonicEnergyRatio", f.create("OddToEvenHarmonicEnergyRatio") },

      { "PitchSalience", f.create("PitchSalience") },
      
      { "SpectrumCQ", f.create("SpectrumCQ") },

      { "TonalExtractor", f.create("TonalExtractor") },
      
  //    { "TonicIndianArtMusic", f.create("TonicIndianArtMusic") },

      { "Tristimulus", f.create("Tristimulus") },
      
      { "TuningFrequency", f.create("TuningFrequency") },

      { "TuningFrequencyExtractor", f.create("TuningFrequencyExtractor") },
      
      { "Chromaprinter", f.create("Chromaprinter") },

      // Audio Problems
      
      { "ClickDetector", f.create("ClickDetector") },

      // Discontinuity Detector
      //    Inputs:
      //    frame (vector_real) - the input frame (must be non-empty)
      //
      //    Outputs:
      //    discontinuityLocations (vector_real) - the index of the detected discontinuities (if any)
      //    discontinuityAmplitudes (vector_real) - the peak values of the prediction error for the discontinuities (if any)
      //
      //    Parameters:
      //    detectionThreshold (real ∈ [1, ∞), default = : 'detectionThreshold' times the standard deviation plus the median of the frame is used as detection threshold
      //    energyThreshold (real ∈ (-∞, ∞), default = -60) : threshold in dB to detect silent subframes
      //    frameSize (integer ∈ (0, ∞), default = 512) : the expected size of the input audio signal (this is an optional parameter to optimize memory allocation)
      //    hopSize (integer ∈ [0, ∞), default = 256) : hop size used for the analysis. This parameter must be set correctly as it cannot be obtained from the input data
      //    kernelSize (integer ∈ [1, ∞), default = 7) : scalar giving the size of the median filter window. Must be odd
      //    order (integer ∈ [1, ∞), default = 3) : scalar giving the number of LPCs to use
      //    silenceThreshold (integer ∈ (-∞, 0), default = -50) : threshold to skip silent frames
      //    subFrameSize (integer ∈ [1, ∞), default = 32) : size of the window used to compute silent subframes
      { "DiscontinuityDetector", f.create("DiscontinuityDetector") },

      { "FalseStereoDetector", f.create("FalseStereoDetector") },
      
      { "GapsDetector", f.create("GapsDetector") },

      { "HumDetector", f.create("HumDetector") },
      
      { "NoiseBurstDetector", f.create("NoiseBurstDetector") },

      { "SNR", f.create("SNR") },
      
      { "SaturationDetector", f.create("SaturationDetector") },

      { "StartStopCut", f.create("StartStopCut") },
      
      { "TruePeakDetector", f.create("TruePeakDetector") },

      { "Duration", f.create("Duration") },
      
      { "EffectiveDuration", f.create("EffectiveDuration") },

      { "FadeDetection", f.create("FadeDetection") },
      
      { "SilenceRate", f.create("SilenceRate") },

      { "StartStopSilence", f.create("StartStopSilence") },
      
      // Loudness/dynamics
      
      { "DynamicComplexity", f.create("DynamicComplexity") },
      
  //    { "Intensity", f.create("Intensity") },

      // standard-mode only
      //    { "Larm", f.create("Larm") },

      { "Leq", f.create("Leq") },
      
      { "LevelExtractor", f.create("LevelExtractor") },

      { "Loudness", f.create("Loudness") },
      
      { "LoudnessEBUR128", f.create("LoudnessEBUR128") },

      { "LoudnessEBUR128Filter", f.create("LoudnessEBUR128Filter") },
      
      { "LoudnessVickers", f.create("LoudnessVickers") },

      { "ReplayGain", f.create("ReplayGain") },
      
      // Extractors
      { "BarkExtractor", f.create("BarkExtractor") },
      
  //    { "Extractor", f.create("Extractor") },
      
  //    { "FreesoundExtractor", f.create("FreesoundExtractor") },

      { "LowLevelSpectralEqloudExtractor", f.create("LowLevelSpectralEqloudExtractor") },
      
      { "LowLevelSpectralExtractor", f.create("LowLevelSpectralExtractor") },
      
      // Synthesis
      { "HarmonicMask", f.create("HarmonicMask") },
      
      { "HarmonicModelAnal", f.create("HarmonicModelAnal") },

      { "HprModelAnal", f.create("HprModelAnal") },
      
      { "HpsModelAnal", f.create("HpsModelAnal") },

      { "ResampleFFT", f.create("ResampleFFT") },
      
      { "SineModelAnal", f.create("SineModelAnal") },
      
      { "SineModelSynth", f.create("SineModelSynth") },
      
      { "SineSubtraction", f.create("SineSubtraction") },

      { "SprModelAnal", f.create("SprModelAnal") },
      
      { "SprModelSynth", f.create("SprModelSynth") },

      { "SpsModelAnal", f.create("SpsModelAnal") },
      
      { "SpsModelSynth", f.create("SpsModelSynth") },

      { "StochasticModelAnal", f.create("StochasticModelAnal") },
      
      { "StochasticModelSynth", f.create("StochasticModelSynth") },

      // Pitch
      { "MultiPitchMelodia", f.create("MultiPitchMelodia") },
      
      { "PitchContours", f.create("PitchContours") },
      
      { "PitchContoursMelody", f.create("PitchContoursMelody") },
      
      { "PitchContoursMonoMelody", f.create("PitchContoursMonoMelody") },
      
      { "PitchContoursMultiMelody", f.create("PitchContoursMultiMelody") },
      
      { "PitchFilter", f.create("PitchFilter") },
      
      { "PitchMelodia", f.create("PitchMelodia") },
      
      { "PitchSalienceFunction", f.create("PitchSalienceFunction") },
      
      { "PitchSalienceFunctionPeaks", f.create("PitchSalienceFunctionPeaks") },
      
      { "PitchYin", f.create("PitchYin") },
      
      { "PitchYinFFT", f.create("PitchYinFFT") },
      
      { "PitchYinProbabilistic", f.create("PitchYinProbabilistic") },
      
      { "PitchYinProbabilities", f.create("PitchYinProbabilities") },
      
      { "PitchYinProbabilitiesHMM", f.create("PitchYinProbabilitiesHMM") },
      
      { "PredominantPitchMelodia", f.create("PredominantPitchMelodia") },
      
      { "Vibrato", f.create("Vibrato") },
      
  //    Standard Mode Only
  //    { "PCA", f.create("PCA") },

      // Segmentation
      { "SBic", f.create("SBic") },
      
      { "SpectralComplexity", f.create("SpectralComplexity") },
      
      { "Spectrum", f.create("Spectrum") },
      
      { "SpectralPeaks", f.create("SpectralPeaks")},
      
      { "RMS",  f.create("RMS") },
      
      { "HPCP", f.create("HPCP") },
      
      { "PoolAggregator", f.create("PoolAggregator")},
      
      { "BeatsLoudness", f.create("BeatsLoudness") },
      
      { "Beatogram", f.create("Beatogram") },
      
      { "Energy",  f.create("Energy") },
      
      { "InstantPower",  f.create("InstantPower") },
      
      { "Centroid",  f.create("Centroid", "range", sampleRate/2) },
      
      { "MFCC", f.create("MFCC",
                               "normalize", "unit_sum",
                               "highFrequencyBound", 12000) },
    };
  
  // if a file is passed, load it into one of essentia's MonoLoader objects
  // which creates a mono data stream, demuxing stereo if needed.
  if (fileName.length() > 0) {
    newAlgorithms["MonoLoader"] = f.create("MonoLoader",
                                           "filename", fileName,
                                           "sampleRate", sampleRate);
  }

  for (auto pair : newAlgorithms) {
    algorithms.insert(pair);
  }
}

void MLTK::connectAlgorithmStream(VectorInput<Real>* inputVec,
                                  map<string, Algorithm*>& algorithms,
                                  Pool& pool) {
  std::cout << "-------- connecting algorithm stream --------" << std::endl;

  // We start with the incoming signal that was attached to inputVec
  *inputVec >> algorithms["DCRemoval"]->input("signal");

//  *inputVec >> monoAlgorithms["CubicSpline"]->input("x");

//  monoAlgorithms["CubicSpline"]->output("y") >> PC(pool, "CubicSpline.y");
//  monoAlgorithms["CubicSpline"]->output("dy") >> PC(pool, "CubicSpline.dy");;
//  monoAlgorithms["CubicSpline"]->output("ddy") >> PC(pool, "CubicSpline.ddy");;
  
  // Remember that all the strings match 1:1 with Essentia's reference documentation.
  // Algorithms can have an unlimited number of OUTPUTS but every input must
  // always have exactly 1 connection.
  // (tl;dr; inputs always need to be connected)
  algorithms["DCRemoval"]->output("signal") >> algorithms["FrameCutter"]->input("signal");
//  algorithms["DCRemoval"]->output("signal") >> algorithms["LargeFrameCutter"]->input("signal");
  algorithms["FrameCutter"]->output("frame") >> algorithms["Windowing"]->input("frame");
//  algorithms["LargeFrameCutter"]->output("frame") >> algorithms["LargeWindowing"]->input("frame");
  algorithms["Windowing"]->output("frame") >> algorithms["RMS"]->input("array");
  algorithms["Windowing"]->output("frame") >> algorithms["Spectrum"]->input("frame");
//  algorithms["LargeWindowing"]->output("frame") >> algorithms["Chromagram"]->input("frame");
//  algorithms["Chromagram"]->output("chromagram") >> PC(pool, "chromagram");
  algorithms["Spectrum"]->output("spectrum")  >> algorithms["MFCC"]->input("spectrum");
//  algorithms["Spectrum"]->output("spectrum") >> algorithms["SpectralPeaks"]->input("spectrum");
//  algorithms["SpectralPeaks"]->output("frequencies") >> algorithms["HPCP"]->input("frequencies");
//  algorithms["SpectralPeaks"]->output("magnitudes") >> algorithms["HPCP"]->input("magnitudes");

  // Pool Outputs
  algorithms["DCRemoval"]->output("signal") >> PC(pool, "DCRemoval");
  algorithms["FrameCutter"]->output("frame") >> PC(pool, "FrameCutter");
//  algorithms["Windowing"]->output("frame") >> PC(pool, "Windowing");
  algorithms["RMS"]->output("rms") >> PC(pool, "RMS");
  algorithms["Spectrum"]->output("spectrum")  >>  PC(pool, "Spectrum");
  algorithms["MFCC"]->output("mfcc") >> PC(pool, "MFCC.coefs");
  algorithms["MFCC"]->output("bands") >> PC(pool, "MFCC.bands");
//  algorithms["HPCP"]->output("hpcp") >> PC(pool, "HPCP");
}

void MLTK::setup(int frameSize=1024, int sampleRate=44100, int hopSize=512){
  this->frameSize = frameSize;
  this->sampleRate = sampleRate;
  this->hopSize = hopSize;

  monoAudioBuffer.resize(frameSize, 0.0);
  for (int i = 0; i < numberOfInputChannels; i++) {
    // HACKHACK: questionable... because ofApp::audioIn should size it?
    channelSoundBuffers[i].getBuffer().resize(frameSize, 0.0);
    channelAudioBuffers[i].resize(frameSize, 0.0);
  }

  essentia::init();
  essentia::streaming::AlgorithmFactory& factory = essentia::streaming::AlgorithmFactory::instance();
  factory.init();

  setupAlgorithms(factory, monoInputVec, monoAudioBuffer, monoAlgorithms);
  connectAlgorithmStream(monoInputVec, monoAlgorithms, monoPool);

  for (int i = 0; i < numberOfInputChannels; i++) {
    setupAlgorithms(factory, channelInputVectors[i], channelAudioBuffers[i], chAlgorithms[i]);
    connectAlgorithmStream(channelInputVectors[i], chAlgorithms[i], chPools[i]);
  }

  factory.shutdown();

  monoNetwork = new scheduler::Network(monoInputVec);
  monoNetwork->run();

  for (int i = 0; i < numberOfInputChannels; i++) {
    chNetworks[i] = new scheduler::Network(channelInputVectors[i]);
    chNetworks[i]->run();
  }
}

void MLTK::run(){
  update();

  if (!accumulating) {
    monoPool.clear();
    for (int i = 0; i < numberOfInputChannels; i++) {
      chPools[i].clear();
    }
  }

  monoNetwork->reset();
  monoNetwork->run();

  for (int i = 0; i < numberOfInputChannels; i++) {
    chNetworks[i]->reset();
    chNetworks[i]->run();
  }

  if (recording) {
    aggr->input("input").set(monoPool);
    aggr->output("output").set(monoPoolAggr);
    aggr->compute();

    for (int i = 0; i < numberOfInputChannels; i++) {
      chAggr[i]->input("input").set(chPools[i]);
      chAggr[i]->input("output").set(chPoolAggrs[i]);
      chAggr[i]->compute();
    }
  }
}

template <class mType>
bool MLTK::exists(string algorithm, int channel){
  return (channel < 0 ? monoPool : chPools[channel]).contains<mType>(algorithm);
};

vector<Real> MLTK::getData(string algorithm, int channel){
  return (channel < 0 ? monoPool : chPools[channel]).value<vector<vector<Real>>>(algorithm)[0];
};

vector<vector<Real>> MLTK::getRaw(string algorithm, int channel){
  return (channel < 0 ? monoPool : chPools[channel]).value<vector<vector<Real>>>(algorithm);
};

Real MLTK::getValue(string algorithm, int channel){
  return (channel < 0 ? monoPool : chPools[channel]).value<vector<Real>>(algorithm)[0];
};

void MLTK::update(){
  for (int i = 0; i < frameSize; i++){
    float accum = 0.0;
    
    for (int j = 0; j < numberOfInputChannels; j++) {
      // copy values from ofApp to MLTK
      if (channelSoundBuffers[j].size() == frameSize) {
        channelAudioBuffers[j][i] = (Real) channelSoundBuffers[j][i];
        accum += channelAudioBuffers[j][i];
      }
    }

    monoAudioBuffer[i] = (Real) accum / numberOfInputChannels;
  }
}

void MLTK::save(){
  output->input("pool").set(monoPoolAggr);
  output->compute();
}

void MLTK::exit(){
  if (monoNetwork != NULL) {
    monoNetwork->clear();
  }
  monoPool.clear();
  monoPoolAggr.clear();
  monoPoolStats.clear();

  for (int i = 0; i < numberOfInputChannels; i++) {
    chPools[i].clear();
    chPoolAggrs[i].clear();
    chPoolStats.clear();
  }

  essentia::shutdown();
  delete aggr, output, monoNetwork, monoInputVec, chAggr, chOutput, chNetworks, channelInputVectors;
}
