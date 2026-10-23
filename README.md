# APO Soundboard — Scratch Edition v1.0.0

High-fidelity **Windows x64 VST2** soundboard for Equalizer APO, specifically intended for the microphone capture chain. This is the final high-fidelity edition based directly on the supplied Scratch Edition **v0.9.10** source. The low-end/lightweight v0.9.0 edition remains entirely separate.

**Status: final v1.0.0 — Windows-tested and [published](https://github.com/Syed1611/EqualizerAPO-Soundboard/releases/tag/scratch-v1.0.0).** The lightweight edition stays on v0.9.0 and is not replaced.

## New in v1.0.0

- **Official Windows app icon:** the approved cyan waveform with red scrub cursor is embedded as multi-size native RT_ICON / RT_GROUP_ICON resources in the **controller EXE**. The actual window class and running window receive separate small/large icons for the title bar, taskbar and Alt+Tab. The icon also appears in Explorer for the EXE. `AppIcon.png` and `AppIcon.ico` are included.
- **File identity:** both EXE and DLL have Windows VERSIONINFO metadata identifying version `1.0.0.0` and the Scratch Edition.
- **Monitor resilience:** if the WinMM playback device stops accepting writes, local monitoring is disabled and the controller shows a device-disconnected error instead of repeatedly writing. This does **not** stop the VST microphone injection. The existing WASAPI failure and recovery paths are preserved.
- **Diagnostics clarity:** host ring underruns, host output clipped frames, local monitor underruns/overruns, and playback-device errors are identified separately.
- **UI consistency:** changing the waveform editing mode immediately refreshes selected-pad information, file info and waveform display; the working Light/Dark appearance and Diagnostics layout remain.
- **Build separation:** VST DLL and controller both report `BUILD_VERSION=100000`, communicate over the private Scratch `_v10` pipe, and retain the Scratch-only configuration format. Do **not** mix with previous Scratch executables.
- **DSP unchanged:** `src/scratch_dsp.h` is **byte-for-byte identical** to v0.9.10. No alteration to the approved full-spectrum sound quality, scratch synthesis, Pitch, or Auto Sweep.

## Retained Scrub behavior

- 16 customizable sound pads, per-pad hotkeys, live volume/pitch, Auto Sweep and optional local monitoring.
- **Select Range** edits playback range; **Scrub** controls the high-fidelity waveform.
- In **Scrub**, left mouse down immediately takes over the selected pad, even if it was playing.
- Drag right/left to scrub forward/backward; hold still to sustain/freeze the sound.
- Release the left mouse button to pause at the released cursor position.
- **Play/Pause** and **Spacebar** resume/pause from the saved position, **Stop** resets the selected pad.
- Spacebar toggles **only** on Soundboard → Scrub with controller focus and without an active text/numeric/hotkey edit; other pads are unaffected.
- Clear Playing/Scrubbing/Holding/Paused waveform states, mouse-position retention and short transition fades.
- No duplicate playback from merely entering Scrub; v0.9.9's feedback/harsh buzz is fixed as of v0.9.10 and retained.

## Installation

1. Back up your existing Scratch Edition settings if desired, and **fully close the old controller**.
2. Extract this complete folder to a location the VST host and controller can read.
3. In **Equalizer APO Configuration Editor**, add/select a VST on the **microphone/capture** device chain and point it to `APOSoundboardScratch_v1.0.0.dll`.
4. Keep `APOSoundboardScratchController.exe` beside the DLL and use the panel/open-controller command or start it directly.
5. Test loaded pads, hotkeys, Scrub → release → Play/Space, monitor ON/OFF, and the microphone capture path on your Windows system.
6. Old versions use a different communication pipe: use the matching v1.0.0 EXE/DLL together.

This package contains the Windows binaries, C sources, icon PNG/ICO/native resources, tests, build instructions and checksums. The icon in this package is **embedded in the EXE**; you do not need the loose ICO file at runtime.

## Validation / limits

- Windows x64 freestanding C binaries cross-compiled with Clang/LLD and inspected as PE32+; VST `VSTPluginMain` export verified.
- Ten supplied offline test executables passed: eight full DSP suites, repeated scrub re-entry, and multi-pad transport. `test_scratch` ran its available checks but skipped an optional external `.raw` voice fixture not supplied here.
- Clang static analyzer returned no issues for controller and DLL source.
- App icon RT_GROUP_ICON (ID 101) and VERSIONINFO checked in the PE resource directory; full `.ico` includes sizes 16, 24, 32, 48, 64, 128, and 256 pixels.
- **Windows user acceptance:** The maintainer confirmed on 2026-10-23 that v1.0.0 runs correctly on their Windows setup. This validates their normal workflow, not every device, host version, driver or edge case.
- **Not independently verified on Windows:** complete device unplug/replug matrix, DPI configurations, malware scanning and all driver combinations. The offline build and static-analysis results remain as recorded above.
- **Release status:** v1.0.0 is the final Scratch Edition and is [published on GitHub](https://github.com/Syed1611/EqualizerAPO-Soundboard/releases/tag/scratch-v1.0.0) as the current Latest release. The tested release tag points to the final source commit `2ae349f`.

See `BUILDING.txt`, `TESTING.txt`, `AUDIT.txt`, and `CHANGELOG.md`. This project is independent of Equalizer APO; source is MIT licensed.
