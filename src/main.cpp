#include <Arduino.h>
#include <M5Unified.h>

namespace {
constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t UPTIME_LOG_INTERVAL_MS = 1000;
constexpr float TEST_TONE_FREQUENCY_HZ = 440.0f;
constexpr uint32_t TEST_TONE_DURATION_MS = 5000;
constexpr uint8_t SPEAKER_CHANNEL = 0;
constexpr uint8_t VOLUME_LEVELS[] = {32, 64, 96, 128, 160};

size_t selectedVolumeIndex = 2;
uint32_t lastUptimeLogAtMs = 0;
uint32_t toneAttemptCount = 0;
bool lastToneStarted = false;

uint8_t currentVolume() {
  return VOLUME_LEVELS[selectedVolumeIndex];
}

void drawScreen(const char* stateLabel) {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.println("Core Gray");
  M5.Display.println("speaker test");

  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.setTextSize(1);
  M5.Display.println();
  M5.Display.printf("Board id: %d\n", static_cast<int>(M5.getBoard()));
  M5.Display.printf("Freq: %.2f Hz\n", TEST_TONE_FREQUENCY_HZ);
  M5.Display.printf("Duration: %lu ms\n", TEST_TONE_DURATION_MS);
  M5.Display.printf("Volume: %u\n", currentVolume());
  M5.Display.printf("Tone attempts: %lu\n", toneAttemptCount);
  M5.Display.print("Last tone: ");
  M5.Display.println(lastToneStarted ? "started" : "none/failed");
  M5.Display.print("State: ");
  M5.Display.println(stateLabel);
  M5.Display.println();
  M5.Display.println("BtnA: play A4");
  M5.Display.println("BtnB: volume");
}

void logSpeakerSetup() {
  Serial.println();
  Serial.println("M5Stack Core Gray speaker smoke test");
  Serial.println("Firmware booted");
  Serial.printf("board_id=%d\n", static_cast<int>(M5.getBoard()));
  Serial.printf(
      "speaker: backend=m5unified internal_spk=true channel=%u volume=%u test_frequency_hz=%.2f duration_ms=%lu\n",
      SPEAKER_CHANNEL,
      currentVolume(),
      TEST_TONE_FREQUENCY_HZ,
      TEST_TONE_DURATION_MS);
}

void playTestTone(const char* reason) {
  M5.Speaker.setVolume(currentVolume());
  toneAttemptCount++;
  lastToneStarted = M5.Speaker.tone(
      TEST_TONE_FREQUENCY_HZ,
      TEST_TONE_DURATION_MS,
      SPEAKER_CHANNEL,
      true);

  Serial.printf(
      "speaker: tone_start reason=%s ok=%s channel=%u frequency_hz=%.2f duration_ms=%lu volume=%u\n",
      reason,
      lastToneStarted ? "true" : "false",
      SPEAKER_CHANNEL,
      TEST_TONE_FREQUENCY_HZ,
      TEST_TONE_DURATION_MS,
      currentVolume());
  drawScreen(lastToneStarted ? "tone requested" : "tone failed");
}

void cycleVolume() {
  selectedVolumeIndex = (selectedVolumeIndex + 1) %
      (sizeof(VOLUME_LEVELS) / sizeof(VOLUME_LEVELS[0]));
  M5.Speaker.setVolume(currentVolume());

  Serial.printf("speaker: volume_set volume=%u\n", currentVolume());
  drawScreen("volume changed");
}

void logUptime(uint32_t nowMs) {
  if (nowMs - lastUptimeLogAtMs < UPTIME_LOG_INTERVAL_MS) {
    return;
  }

  lastUptimeLogAtMs = nowMs;
  Serial.printf(
      "speaker: uptime_ms=%lu board_id=%d volume=%u tone_attempts=%lu\n",
      nowMs,
      static_cast<int>(M5.getBoard()),
      currentVolume(),
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
  M5.Speaker.setVolume(currentVolume());

  logSpeakerSetup();
  drawScreen("ready");
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    playTestTone("button_a");
  }

  if (M5.BtnB.wasPressed()) {
    cycleVolume();
  }

  logUptime(millis());
  delay(10);
}
