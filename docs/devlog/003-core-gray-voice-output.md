# Slice 003: Core Gray VoiceOutput

The first two slices answered hardware questions directly from `main.cpp`.
That was appropriate while the repo was still proving whether the Core Gray
speaker could produce controlled musical tones. Now that A4/C5/C6 fundamentals
are visible and the local sweep has useful observations, the next question is
architectural: can the Core Gray speaker sit behind the same kind of voice
boundary as the buzzer?

This slice introduces a local `VoiceOutput` interface and an
`M5SpeakerToneOutput` implementation. The interface is intentionally the same
shape as the buzzer experiment's output boundary:

- `startNote(midiNote, frequencyHz, waveform, velocity)`
- `stopNote()`
- `isPlaying()`

The local note sweep still owns the test UI: button handling, selected note,
selected waveform, display text, and serial diagnostics. The M5 speaker output
now owns the M5Unified speaker calls, the speaker channel, the 32-sample square
and saw wave buffers, and the current speaker volume.

The implementation is named `M5SpeakerToneOutput`, not
`CoreGraySpeakerToneOutput`, because the `.cpp` is mostly generic M5Unified
speaker code. The Core Gray-specific part is the validation context and current
default volume, not board-specific branching or pin setup in the implementation.

This is not the shared monophonic instrument extraction yet. The slice only
validates the boundary between "a musical voice request" and "Core Gray speaker
hardware output." If this remains readable after hardware testing, the next
candidate extraction is clearer: a shared monophonic instrument package could
own note priority, pitch bend, and panic/disconnect behavior, while this repo
keeps the Core Gray-specific output.

Verification command:

```bash
pio run
```

Hardware test:

```bash
pio run --target upload
pio device monitor
```

Expected result: the button behavior from slice 002 remains unchanged. Button A
starts/stops the selected tone, Button B advances the note, and Button C toggles
between `square32` and `saw32`. Serial logs should still report note,
frequency, waveform, volume, and output state.

Hardware observations:

- Hardware validation on the Core Gray confirmed that the refactor did not
  change the local sweep behavior. Button A still starts and stops the selected
  tone, Button B still advances the note, Button C still toggles between
  `square32` and `saw32`, and the audible/spectrometer behavior matched the
  previous slice.

Conclusion:

The `VoiceOutput` boundary is now proven locally for the Core Gray speaker
backend. The next useful work is to remove temporary duplication by extracting
well-understood shared pieces such as `MidiNote`, `ToneWaveform`, `VoiceOutput`,
and the monophonic instrument policy into a reusable PlatformIO package.
