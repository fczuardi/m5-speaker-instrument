# M5 speaker instrument

Retired hardware discovery experiments for small M5Stack devices with internal
speakers.

## Retired

This experiment has been migrated into
`/home/fcz/dev/m5stick/monophonic-instrument`.

The useful Core Gray speaker smoke-test behavior now lives in:

- `monophonic-instrument/apps/core-gray-speaker-local-test`
- `monophonic-instrument/packages/m5-tone-output`
- `M5CoreGrayToneOutput`

The migrated firmware was build-validated and hardware-tested on the M5Stack
Core Gray. This repository is kept only as historical development context and
should not receive new work.

## Historical Context

The first target is the M5Stack Core Gray 1.0. This repository starts with the
speaker itself, not with MIDI transport or reusable instrument packaging. The
initial milestone is to prove that PlatformIO can build firmware for the Core
Gray and that M5Unified can drive a recognizable tone through the internal
speaker.

## Final Status

The final firmware is a Core Gray local note sweep.

- M5Unified initializes the board, display, and internal speaker.
- Button A starts or stops the selected tone.
- Button B advances through C3, C4, A4, C5, and C6.
- Button C alternates between `sq32` and `saw32` waveforms.
- The display and serial monitor report board id, note, frequency, waveform,
  speaker volume, output state, and uptime.

The local sweep drove the speaker through `M5SpeakerToneOutput`, which
implemented a small hardware-neutral `VoiceOutput` interface. That boundary has
now been migrated into the shared `M5ToneOutputCore` and `M5CoreGrayToneOutput`
classes in `monophonic-instrument`.

The hardware passes were successful. A spectrometer check of the A4 test tone
showed a clear fundamental near 439 Hz plus visible harmonics. The later note
sweep refined that result: A4, C5, and C6 showed clear fundamentals, while C3
and C4 were represented mainly by higher harmonics at the tested volume. The
speaker is therefore a promising musical output path, but not a full-range
speaker; its useful response and timbre depend on register, waveform, volume,
room, and measurement setup.

This repository intentionally stops at local hardware discovery. BLE MIDI,
shared firmware contracts, pitch bend, velocity mapping, and package extraction
belong in `monophonic-instrument` and `embedded-music-experiments`.

## Commands

These commands are historical and should not be used for new work:

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
