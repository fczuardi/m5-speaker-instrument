#include "M5SpeakerToneOutput.h"

#include <cstddef>

#include <Arduino.h>
#include <M5Unified.h>

namespace {
struct WaveformDefinition {
  ToneWaveform waveform;
  const uint8_t* samples;
  size_t sampleCount;
};

constexpr uint8_t SQUARE_WAVE_32[] = {
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
};

constexpr uint8_t SAW_WAVE_32[] = {
    0,   8,   16,  25,  33,  41,  49,  58,
    66,  74,  82,  90,  99,  107, 115, 123,
    132, 140, 148, 156, 165, 173, 181, 189,
    197, 206, 214, 222, 230, 239, 247, 255,
};

constexpr WaveformDefinition WAVEFORMS[] = {
    {ToneWaveform::Square32, SQUARE_WAVE_32, sizeof(SQUARE_WAVE_32)},
    {ToneWaveform::Saw32, SAW_WAVE_32, sizeof(SAW_WAVE_32)},
};

const WaveformDefinition& waveformDefinition(ToneWaveform waveform) {
  for (const WaveformDefinition& definition : WAVEFORMS) {
    if (definition.waveform == waveform) {
      return definition;
    }
  }

  return WAVEFORMS[0];
}
}

void M5SpeakerToneOutput::begin() {
  M5.Speaker.begin();
  M5.Speaker.setVolume(volume_);
  initialized_ = true;
  playing_ = false;
}

void M5SpeakerToneOutput::end() {
  if (!initialized_) {
    return;
  }

  stopNote();
  M5.Speaker.end();
  initialized_ = false;
}

bool M5SpeakerToneOutput::startNote(
    uint8_t,
    float frequencyHz,
    ToneWaveform waveform,
    uint8_t) {
  if (!initialized_ || frequencyHz <= 0.0f) {
    return false;
  }

  waveform_ = waveform;
  M5.Speaker.setVolume(volume_);

  const WaveformDefinition& definition = waveformDefinition(waveform_);
  playing_ = M5.Speaker.tone(
      frequencyHz,
      UINT32_MAX,
      SPEAKER_CHANNEL,
      true,
      definition.samples,
      definition.sampleCount);

  return playing_;
}

void M5SpeakerToneOutput::stopNote() {
  if (!initialized_) {
    return;
  }

  M5.Speaker.stop(SPEAKER_CHANNEL);
  playing_ = false;
}

bool M5SpeakerToneOutput::isPlaying() const {
  return playing_;
}

void M5SpeakerToneOutput::setVolume(uint8_t volume) {
  volume_ = volume;
  if (initialized_) {
    M5.Speaker.setVolume(volume_);
  }
}

uint8_t M5SpeakerToneOutput::volume() const {
  return volume_;
}

const char* M5SpeakerToneOutput::waveformName() const {
  return toneWaveformName(waveform_);
}
