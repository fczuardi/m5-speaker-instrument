# M5 speaker instrument

Hardware discovery experiments for small M5Stack devices with internal
speakers.

The first target is the M5Stack Core Gray 1.0. This repository starts with the
speaker itself, not with MIDI transport or reusable instrument packaging. The
initial milestone is to prove that PlatformIO can build firmware for the Core
Gray and that M5Unified can drive a recognizable tone through the internal
speaker.

## Current Status

The current firmware is a Core Gray speaker smoke test.

- M5Unified initializes the board, display, and internal speaker.
- Button A plays a five-second A4 tone for spectrometer inspection.
- Button B cycles through a small set of speaker volume levels.
- The display and serial monitor report board id, speaker volume, tone
  frequency, and uptime.

The first hardware pass was successful. A spectrometer check of the A4 test tone
showed a clear fundamental near 439 Hz plus visible harmonics, which makes the
Core Gray speaker a more promising musical output path than the M5StickC Plus2
buzzer for notes where the fundamental matters.

This is intentionally not a synthesizer yet. BLE MIDI, shared firmware
contracts, pitch bend, velocity mapping, and package extraction are out of scope
until the basic speaker path is observable on hardware.

## Commands

```bash
pio run
pio run --target upload
pio device monitor
```

## Hardware

- M5Stack Core Gray 1.0
- ESP32
- PlatformIO
- Arduino framework
- M5Unified
