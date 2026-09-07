#include "ToneWaveform.h"

const char* toneWaveformName(ToneWaveform waveform) {
  switch (waveform) {
    case ToneWaveform::Square32:
      return "square32";
    case ToneWaveform::Saw32:
      return "saw32";
  }

  return "unknown";
}
