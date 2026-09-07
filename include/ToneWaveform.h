#pragma once

#include <cstdint>

enum class ToneWaveform : uint8_t {
  Square32,
  Saw32,
};

const char* toneWaveformName(ToneWaveform waveform);
