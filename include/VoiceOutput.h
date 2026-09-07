#pragma once

#include <cstdint>

#include "ToneWaveform.h"

// Hardware-neutral output boundary for code that wants to request one voice.
//
// Implementations decide how frequency, waveform, and velocity become physical
// sound on a specific device.
class VoiceOutput {
public:
  virtual ~VoiceOutput() = default;

  virtual bool startNote(
      uint8_t midiNote,
      float frequencyHz,
      ToneWaveform waveform,
      uint8_t velocity) = 0;
  virtual void stopNote() = 0;
  virtual bool isPlaying() const = 0;
};
