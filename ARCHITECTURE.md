# Scratch Edition architecture

Scratch Edition v0.9.8 is the higher-resource alternative to [lightweight v0.9.0](https://github.com/Syed1611/EqualizerAPO-Soundboard/tree/main). They have different DLL/controller pairs, local communication endpoints, and configuration.

## Equalizer APO integration

- **VST2 DLL:** hosted in the Equalizer APO microphone chain; mixes soundboard output into incoming microphone audio.
- **Controller EXE:** 16 pads, global hotkeys, waveform display, file decoding, diagnostics, local monitor, Scratch processing and audio controls.
- **Communication:** local Windows named pipe. Always keep the matching controller and versioned DLL together.
- No virtual cable is needed to inject soundboard audio into the microphone path.

## Waveform controls and audio engine

**Select Range** allows a user to edit the normal playback region, with blue selection shading.

**Scratch** uses mouse movement to traverse source audio forward or backward without automatically changing the musical pitch. At a stationary cursor, it continuously synthesizes the sound around the selected position. The separate Pitch slider and Auto Sweep modulate scratch pitch live.

The high-fidelity Scratch path uses a full-spectrum 2048-point FFT overlap-add processor. When movement is suitable for near-natural-speed direct playback, it can read decoded PCM directly instead. Indefinite holds and extreme stretching are not lossless.

The audio engine in v0.9.8 matches v0.9.7; the later release mainly improves UI behavior.

## Windows UI

- Windows-style Soundboard/Diagnostics tabs.
- Parent-painted diagnostics panel boundaries to avoid overlapping native group-box redraw.
- Coordinated resize and theme invalidation.
- Distinct waveform visuals for Select Range and Scratch.
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
