# Slice 001: Core Gray Speaker Smoke Test

The first goal is hardware discovery. Before adding MIDI, note tracking, or an
instrument abstraction, this slice should answer a simpler question: can this
repo build firmware for the M5Stack Core Gray 1.0, boot it, and produce a
recognizable tone through the internal speaker?

The project starts as a minimal PlatformIO Arduino firmware using M5Unified.
`platformio.ini` targets PlatformIO's `m5stack-core-esp32` board and pins the
same `espressif32` and `M5Unified` versions used by the sibling experiments, so
the first unknown is the Core Gray speaker path rather than dependency drift.
During the first build attempt, the workspace also moved to a shared parent
PlatformIO cache at `/home/fcz/dev/m5stick/.platformio-home` so future sibling
repos do not install duplicate ESP32 toolchains. The first dependency
installation needed `TMPDIR=/home/fcz/dev/m5stick/.tmp` because the default
temporary path hit quota limits while installing `tool-esptoolpy`. That was a
local setup workaround, not part of the project architecture. The checked-in
project configuration keeps the shared `core_dir` and adds `-pipe` so C/C++
compilation puts less pressure on `/tmp`.

The firmware initializes M5Unified with the internal speaker enabled, draws a
small status screen, and logs diagnostic facts over serial. Button A plays a
short A4 tone at 440 Hz. Button B cycles through a few fixed volume levels so
the hardware test can quickly find whether the Core Gray speaker needs a lower
or higher setting to make the pitch recognizable without sounding harsh.

This slice deliberately avoids BLE, shared contracts, pitch bend, velocity
mapping, and package extraction. Those belong after the speaker path is proven.

Verification command:

```bash
pio run
```

Hardware test:

```bash
pio run --target upload
pio device monitor
```

Expected result: the device shows the Core Gray speaker smoke-test screen,
Button A produces a short recognizable A4 tone, Button B changes the volume
level, and the serial monitor prints the board id, volume, tone start result,
and uptime.

Hardware observations:

- Pending first flash to a Core Gray 1.0.
- Initial local build attempts reached PlatformIO dependency installation but
  failed while installing `tool-esptoolpy` because of disk quota pressure from
  duplicate caches.
- After cleaning duplicate caches and populating the shared cache with a
  workspace-local `TMPDIR`, `pio run` built the firmware successfully.
