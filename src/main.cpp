#include <Arduino.h>
#include <M5Unified.h>

namespace {
constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t UPTIME_LOG_INTERVAL_MS = 1000;
constexpr uint8_t SPEAKER_CHANNEL = 0;
constexpr uint8_t SPEAKER_VOLUME = 35;

struct SweepNote {
  const char* name;
  float frequencyHz;
};

struct WaveformDefinition {
  const char* name;
  const uint8_t* samples;
  size_t sampleCount;
};

constexpr SweepNote SWEEP_NOTES[] = {
    {"C3", 130.81f},
    {"C4", 261.63f},
    {"A4", 440.00f},
    {"C5", 523.25f},
    {"C6", 1046.50f},
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
    {"square32", SQUARE_WAVE_32, sizeof(SQUARE_WAVE_32)},
    {"saw32", SAW_WAVE_32, sizeof(SAW_WAVE_32)},
};

size_t selectedNoteIndex = 2;
size_t selectedWaveformIndex = 0;
uint32_t lastUptimeLogAtMs = 0;
uint32_t toneAttemptCount = 0;
bool lastToneStarted = false;
bool tonePlaying = false;

const SweepNote& currentNote() {
  return SWEEP_NOTES[selectedNoteIndex];
}

const WaveformDefinition& currentWaveform() {
  return WAVEFORMS[selectedWaveformIndex];
}

void drawScreen(const char* stateLabel) {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.println("Core Gray");
  M5.Display.println("note sweep");

  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.println();
  M5.Display.printf("Board id: %d\n", static_cast<int>(M5.getBoard()));
  M5.Display.printf("Note: %s\n", currentNote().name);
  M5.Display.printf("Freq: %.2f Hz\n", currentNote().frequencyHz);
  M5.Display.printf("Wave: %s\n", currentWaveform().name);
  M5.Display.printf("Volume: %u\n", SPEAKER_VOLUME);
  M5.Display.printf("Tone attempts: %lu\n", toneAttemptCount);
  M5.Display.print("Output: ");
  M5.Display.println(tonePlaying ? "playing" : "stopped");
  M5.Display.print("Last request: ");
  M5.Display.println(lastToneStarted ? "accepted" : "none/failed");
  M5.Display.print("State: ");
  M5.Display.println(stateLabel);
  M5.Display.println();
  M5.Display.println("BtnA: start/stop");
  M5.Display.println("BtnB: next note");
  M5.Display.println("BtnC: waveform");
}

void logSpeakerSetup() {
  Serial.println();
  Serial.println("M5Stack Core Gray local note sweep");
  Serial.println("Firmware booted");
  Serial.printf("board_id=%d\n", static_cast<int>(M5.getBoard()));
  Serial.printf(
      "speaker: backend=m5unified internal_spk=true channel=%u volume=%u notes=%u waveforms=%u\n",
      SPEAKER_CHANNEL,
      SPEAKER_VOLUME,
      static_cast<unsigned>(sizeof(SWEEP_NOTES) / sizeof(SWEEP_NOTES[0])),
      static_cast<unsigned>(sizeof(WAVEFORMS) / sizeof(WAVEFORMS[0])));
}

void startTone(const char* reason) {
  M5.Speaker.setVolume(SPEAKER_VOLUME);
  toneAttemptCount++;
  lastToneStarted = M5.Speaker.tone(
      currentNote().frequencyHz,
      UINT32_MAX,
      SPEAKER_CHANNEL,
      true,
      currentWaveform().samples,
      currentWaveform().sampleCount);
  tonePlaying = lastToneStarted;

  Serial.printf(
      "speaker: tone_start reason=%s ok=%s channel=%u note=%s frequency_hz=%.2f waveform=%s volume=%u\n",
      reason,
      lastToneStarted ? "true" : "false",
      SPEAKER_CHANNEL,
      currentNote().name,
      currentNote().frequencyHz,
      currentWaveform().name,
      SPEAKER_VOLUME);
  drawScreen(lastToneStarted ? "tone requested" : "tone failed");
}

void stopTone(const char* reason) {
  if (!tonePlaying) {
    return;
  }

  M5.Speaker.stop(SPEAKER_CHANNEL);
  tonePlaying = false;

  Serial.printf(
      "speaker: tone_stop reason=%s channel=%u\n",
      reason,
      SPEAKER_CHANNEL);
  drawScreen("stopped");
}

void toggleTone() {
  if (tonePlaying) {
    stopTone("button_a");
    return;
  }

  startTone("button_a");
}

bool restartToneIfPlaying(const char* reason) {
  if (!tonePlaying) {
    return false;
  }

  M5.Speaker.stop(SPEAKER_CHANNEL);
  tonePlaying = false;
  startTone(reason);
  return true;
}

void selectNextNote() {
  selectedNoteIndex = (selectedNoteIndex + 1) %
      (sizeof(SWEEP_NOTES) / sizeof(SWEEP_NOTES[0]));

  Serial.printf(
      "speaker: note_select note=%s frequency_hz=%.2f\n",
      currentNote().name,
      currentNote().frequencyHz);
  if (!restartToneIfPlaying("note_change")) {
    drawScreen("note selected");
  }
}

void selectNextWaveform() {
  selectedWaveformIndex = (selectedWaveformIndex + 1) %
      (sizeof(WAVEFORMS) / sizeof(WAVEFORMS[0]));

  Serial.printf(
      "speaker: waveform_select waveform=%s\n",
      currentWaveform().name);
  if (!restartToneIfPlaying("waveform_change")) {
    drawScreen("waveform selected");
  }
}

void logUptime(uint32_t nowMs) {
  if (nowMs - lastUptimeLogAtMs < UPTIME_LOG_INTERVAL_MS) {
    return;
  }

  lastUptimeLogAtMs = nowMs;
  Serial.printf(
      "speaker: uptime_ms=%lu board_id=%d note=%s frequency_hz=%.2f waveform=%s volume=%u playing=%s tone_attempts=%lu\n",
      nowMs,
      static_cast<int>(M5.getBoard()),
      currentNote().name,
      currentNote().frequencyHz,
      currentWaveform().name,
      SPEAKER_VOLUME,
      tonePlaying ? "true" : "false",
      toneAttemptCount);
}

}

void setup() {
  auto config = M5.config();
  config.internal_spk = true;
  config.internal_mic = false;
  M5.begin(config);

  Serial.begin(SERIAL_BAUD);
  delay(200);

  M5.Display.setRotation(1);
  M5.Display.setBrightness(96);

  M5.Speaker.begin();
  M5.Speaker.setVolume(SPEAKER_VOLUME);

  logSpeakerSetup();
  drawScreen("ready");
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    toggleTone();
  }

  if (M5.BtnB.wasPressed()) {
    selectNextNote();
  }

  if (M5.BtnC.wasPressed()) {
    selectNextWaveform();
  }

  const uint32_t nowMs = millis();
  logUptime(nowMs);
  delay(10);
}
