# Slice 002: Core Gray Local Note Sweep

The first smoke test proved that A4 can produce a visible fundamental on the
Core Gray speaker. The next useful question is whether that result holds across
a small musical range and whether different simple waveforms change the
spectrometer picture in a useful way.

This slice stays local to the device. It still avoids BLE MIDI, shared
contracts, pitch bend, velocity mapping, and package extraction. The goal is
hardware evidence, not instrument architecture.

The firmware now turns the three Core Gray buttons into a small test panel:

- Button A starts or stops the selected tone.
- Button B advances through C3, C4, A4, C5, and C6.
- Button C alternates between `square32` and `saw32`.

The button handling follows the official Basic/Gray M5Unified example:
`M5.update()` is called in the main loop, and the firmware checks
`M5.BtnA.wasPressed()`, `M5.BtnB.wasPressed()`, and `M5.BtnC.wasPressed()` for
edge-triggered local controls.

The tone is started with an explicit open-ended duration and stopped with
`M5.Speaker.stop()`. That makes this slice test start and stop control directly
instead of relying only on a fixed timeout. If the tone is already playing,
changing the note or waveform restarts playback immediately so the spectrometer
can compare the new selection without an extra button press.

The sweep uses one fixed speaker volume. It started at `96` because the
previous A4 smoke test already proved that volume was useful enough for first
inspection. The first sweep pass sounded a little loud, so the follow-up check
uses `35`, which was more comfortable for inspecting C3 in the room. A later
slice can make volume calibration its own experiment if the note sweep shows
that the Core Gray speaker is worth pursuing as an instrument output.

Verification command:

```bash
pio run
```

Hardware test:

```bash
pio run --target upload
pio device monitor
```

Observation checklist:

- For each note, record whether the fundamental is visible.
- Record whether harmonics dominate the fundamental.
- Note obvious distortion, rattling, or unstable pitch.
- Compare `square32` and `saw32` at the same note and volume.

Hardware observations:

- The first note sweep waterfall screenshot showed clear vertical harmonic
  stacks across the tested notes. The overtone structure looked musically
  useful, but the fixed volume `96` was a little loud for the room/test setup.
- In the C3 checks at volume `35`, the expected fundamental near 130 Hz was not
  visible in the waterfall. The first faint visible line appeared around 520 Hz,
  with another faint line around 650 Hz. Stronger lines started around 770 Hz,
  914 Hz, 1043 Hz, and continued upward. One square-wave screenshot had a marked
  peak around 2203 Hz, and one saw-wave screenshot had a marked peak around
  1758 Hz.
- A five-note saw-wave sweep screenshot at volume `35`, starting from C3,
  showed distinct ascending harmonic stacks for the selected notes. The pattern
  looked controlled and musically organized across the sweep. The most visible
  energy was still in harmonic partials rather than in the low fundamentals,
  but the notes were clearly separated in the waterfall.
- A separate Android guitar tuner check detected A4 as in tune. That suggests
  the generated pitch is basically correct, even when the C3 waterfall view does
  not show the low fundamental directly.

Saw-wave fundamental visibility on the Android spectrometer app:

| Note | Expected fundamental | Fundamental visible? | First visible line |
| --- | ---: | --- | ---: |
| C3 | 130 Hz | no | about 539 Hz |
| C4 | 261.63 Hz | no | about 521 Hz |
| A4 | 440 Hz | yes | about 440 Hz |
| C5 | 523.25 Hz | yes | about 523.5 Hz |
| C6 | 1046.50 Hz | yes | about 1046 Hz |

The saw-wave result suggests that the Core Gray speaker path is useful from A4
upward in this room/app setup, while C3 and C4 are still represented mainly by
harmonics rather than by visible low fundamentals.

Square-wave fundamental visibility on the Android spectrometer app:

| Note | Expected fundamental | Fundamental visible? | First visible line |
| --- | ---: | --- | ---: |
| C3 | 130 Hz | no | about 656 Hz |
| C4 | 261.63 Hz | barely | about 785 Hz clearly |
| A4 | 440 Hz | yes | about 440 Hz |
| C5 | 523.25 Hz | yes | about 523 Hz |
| C6 | 1046.50 Hz | yes | about 1046 Hz |

The square-wave result is similar above A4. In the lower notes it does not solve
the missing fundamental problem: C3 still appears through higher harmonics, and
C4 only has a barely visible fundamental before the clearer upper partials.
