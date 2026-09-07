#pragma once

#include <cstdint>

#include "ToneWaveform.h"
#include "VoiceOutput.h"

// Drives an M5Unified internal speaker output.
//
// This class owns M5.Speaker tone playback and calibration. It intentionally
// does not own note selection, MIDI handling, monophonic priority, or display
// state. The current default volume is validated on the M5Stack Core Gray 1.0.
class M5SpeakerToneOutput : public VoiceOutput {
public:
  void begin();
  void end();

  bool startNote(
      uint8_t midiNote,
      float frequencyHz,
      ToneWaveform waveform,
      uint8_t velocity) override;
  void stopNote() override;
  bool isPlaying() const override;

  void setVolume(uint8_t volume);
  uint8_t volume() const;
  const char* waveformName() const;

private:
  static constexpr uint8_t SPEAKER_CHANNEL = 0;

  bool initialized_ = false;
  bool playing_ = false;
  uint8_t volume_ = 35;
  ToneWaveform waveform_ = ToneWaveform::Square32;
};
