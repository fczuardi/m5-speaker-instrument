# M5 speaker instrument

Hardware discovery experiments for small M5Stack devices with internal
speakers.

The first target is the M5Stack Core Gray 1.0. This repository starts with the
speaker itself, not with MIDI transport or reusable instrument packaging. The
initial milestone is to prove that PlatformIO can build firmware for the Core
Gray and that M5Unified can drive a recognizable tone through the internal
speaker.

## Current Status

The current firmware is a Core Gray local note sweep.

- M5Unified initializes the board, display, and internal speaker.
- Button A starts or stops the selected tone.
- Button B advances through C3, C4, A4, C5, and C6.
- Button C alternates between `square32` and `saw32` waveforms.
- The display and serial monitor report board id, note, frequency, waveform,
  speaker volume, output state, and uptime.

The local sweep drives the speaker through `M5SpeakerToneOutput`, which
implements a small hardware-neutral `VoiceOutput` interface. This keeps
M5Unified speaker playback and Core Gray-validated calibration separate from
the button UI, and prepares the repo to test whether the same monophonic
instrument policy used by the buzzer can be shared later.

The hardware passes were successful. A spectrometer check of the A4 test tone
showed a clear fundamental near 439 Hz plus visible harmonics. The later note
sweep refined that result: A4, C5, and C6 showed clear fundamentals, while C3
and C4 were represented mainly by higher harmonics at the tested volume. The
speaker is therefore a promising musical output path, but not a full-range
speaker; its useful response and timbre depend on register, waveform, volume,
room, and measurement setup.

This is intentionally not a synthesizer yet. BLE MIDI, shared firmware
contracts, pitch bend, velocity mapping, and package extraction remain outside
this local hardware-discovery slice. The basic speaker path is now observable,
so later slices can introduce those concerns one boundary at a time.

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
