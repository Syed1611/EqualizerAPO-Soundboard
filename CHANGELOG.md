# Scratch Edition changelog

## v1.0.0 — Application Branding and Stability Polish

- Embedded waveform/red-cursor logo as multiple native Win32 icon resources in the controller EXE.
- Assigned icon to the registered window class and running window for small/large UI and Explorer.
- Added VERSIONINFO `1.0.0.0` to DLL and EXE.
- Improved WinMM monitor-device write failure handling: monitoring disables cleanly and reports a recoverable device error; microphone injection remains active.
- Clarified independent VST ring underrun, host output clip, and monitor device diagnostics.
- Refreshes waveform and selected-pad status immediately when changing mode.
- Locked both binaries to build 100000, `Scratch_v10` pipe and matching source.
- Preserved all of v0.9.10 Scrub controls and bit-identical full-spectrum spectral DSP.
- Offline tests and Win64 cross-compilation passed. On-machine Windows validation remains necessary.

## v0.9.10 — Repeated Scrub Audio Hotfix

- Corrected mismatched scratch-entry fade duration and negative blend weights.
- Removed previous-output feedback from scrub entry; restored decoded PCM at click.
- Covered repeated left-click/drag/release/Play interactions with regression tests.

## v0.9.9 — Scrub Playback Controls

- Renamed the waveform mode to Scrub, introduced click takeover, pause-on-release, Play/Pause/Stop and Scrub-only selected-pad Spacebar.
- Superseded by v0.9.10 due to an audio entry fade regression.

## v0.9.8 — Prior High-Fidelity Baseline

- The full-spectrum spectral scratch engine and Windows-style UI refinements are retained through v1.0.0.
