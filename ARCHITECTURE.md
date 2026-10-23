# Scratch Edition architecture

Scratch Edition v1.0.0 is the higher-resource alternative to [lightweight v0.9.0](https://github.com/Syed1611/EqualizerAPO-Soundboard/tree/main). They have different DLL/controller pairs, local communication endpoints, and configuration.

## Equalizer APO integration

- **VST2 DLL:** hosted in the Equalizer APO microphone chain; mixes soundboard output into incoming microphone audio.
- **Controller EXE:** 16 pads, global hotkeys, waveform display, file decoding, diagnostics, local monitor, Scratch processing and audio controls.
- **Communication:** local Windows named pipe. Always keep the matching controller and versioned DLL together.
- No virtual cable is needed to inject soundboard audio into the microphone path.

## Waveform controls and audio engine

**Select Range** allows a user to edit the normal playback region, with blue selection shading.

**Scrub** uses mouse movement to traverse source audio forward or backward without automatically changing the musical pitch. At a stationary cursor, it continuously synthesizes the sound around the selected position. The separate Pitch slider and Auto Sweep modulate scratch pitch live.

The high-fidelity Scratch path uses a full-spectrum 2048-point FFT overlap-add processor. When movement is suitable for near-natural-speed direct playback, it can read decoded PCM directly instead. Indefinite holds and extreme stretching are not lossless.

The v0.9.8 full-spectrum synthesis is retained. v0.9.9 added per-pad Scrub playback control, v0.9.10 fixed a scrub re-entry fade regression, and v1.0.0 adds Windows icon resources, version information, monitor-device resilience, and diagnostics clarity.

## Windows UI

- Windows-style Soundboard/Diagnostics tabs.
- Parent-painted diagnostics panel boundaries to avoid overlapping native group-box redraw.
- Coordinated resize and theme invalidation.
- Distinct waveform visuals for Select Range and Scrub.
- Bold, centered Speed/Depth numeric fields.

## Code and testing

- `src/controller.c`: UI, audio, named-pipe communication, and monitoring.
- `src/scratch_dsp.h`: scratch/freeze DSP.
- `src/plugin.c`: VST2 host integration and microphone mixing.
- `src/winlite.h`: freestanding Win32 API declarations.
- `tests/`: offline audio fidelity, motion, freeze, pitch, and rate-conversion regressions.

See [BUILDING.txt](BUILDING.txt), [TESTING.txt](TESTING.txt), [AUDIT.txt](AUDIT.txt) and [CHANGELOG.md](CHANGELOG.md).

## Historical source

Every released source snapshot can be accessed by its published `scratch-v0.9.1` through `scratch-v0.9.8` tags. See [SCRATCH_RELEASES.md](SCRATCH_RELEASES.md); no additional history branch is needed.

## License

MIT. This project is not affiliated with Equalizer APO.

## v1.0.0 final edition

- `AppIcon.ico`, `AppIcon.png`: waveform/red-cursor branding.
- `AppIcon.res`, `PluginVersion.res`: Windows RT_GROUP_ICON and VERSIONINFO resources for the matched release binaries.
- `BUILD_VERSION=100000` and `Scratch_v10` named pipe; use the matched controller and VST DLL.
- `tests/test_transport.c` and `tests/test_scrub_reentry.c` cover per-pad control and re-entry safeguards.
- `src/scratch_dsp.h` remains unchanged from the supplied v0.9.10 build.
- The maintainer confirmed the v1.0.0 build works in their Windows environment on 2026-10-23. The GitHub Release will be published separately as a **non-prerelease**; broader device/driver compatibility remains unverified.
