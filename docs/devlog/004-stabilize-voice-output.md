# Slice 004: Stabilize VoiceOutput Behavior

The `M5SpeakerToneOutput` refactor made the Core Gray speaker look like the
buzzer output at the interface level, but the first review found two important
details before extraction.

First, `startNote()` had a state bug for invalid requests. If a tone was already
playing and a caller requested an invalid frequency, the method returned
`false` and changed `playing_` to `false` without stopping the speaker. That
could make `isPlaying()` disagree with the hardware. This slice matches the
buzzer output semantics: an invalid `startNote()` request returns `false`
without changing the current output state. Stopping remains an explicit
`stopNote()` operation.

Second, the duplicated `ToneWaveform` labels had already diverged. The buzzer
reported `Square32` as `sq32`, while the speaker repo reported `square32`. This
slice standardizes the speaker label to `sq32` so both outputs use the same
diagnostic vocabulary while the code is still duplicated.

The comparison between the two current outputs is now:

| Behavior | Buzzer output | M5 speaker output |
| --- | --- | --- |
| Valid `startNote()` | starts requested tone | starts requested tone |
| Invalid `startNote()` while stopped | returns `false`, stays stopped | returns `false`, stays stopped |
| Invalid `startNote()` while playing | returns `false`, keeps current state | returns `false`, keeps current state |
| `stopNote()` | stops current output | stops current output |
| `end()` | stops, ends speaker, marks uninitialized | stops, ends speaker, marks uninitialized |
| Square label | `sq32` | `sq32` |
| Saw label | `saw32` | `saw32` |

This slice also adds minimal GitHub Actions CI. The repo still has no native
tests because the output depends directly on `M5.Speaker`, and introducing a
mockable adapter would be a separate design choice. For now, CI validates the
firmware build with PlatformIO. Local hardware validation remains necessary for
audio behavior.

Proposed smallest shared package after this stabilization:

- `MidiNote`
- `ToneWaveform`
- `VoiceOutput`
- `MonophonicInstrument`
- `MonophonicInstrumentSink`

One open naming issue remains: `Square32` and `Saw32` describe the current
32-sample wavetable representation. Before freezing a shared contract, the next
extraction slice should decide whether the public enum should instead be
`Square` and `Saw`, leaving each backend to choose its rendering method.

Verification command:

```bash
pio run
```

Hardware observations:

- Pending hardware check after the invalid-request semantics and diagnostic
  label cleanup.
