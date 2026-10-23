# Scratch Edition v1.0.0 — Final Release Instructions

**Status:** final high-fidelity edition, accepted in the maintainer's Windows environment on 2026-10-23. Its cross-compiled DLL/controller and offline regression checks passed. Full device-driver, DPI and monitor-failure test coverage is not claimed.

## Create the final GitHub Release

1. Visit https://github.com/Syed1611/EqualizerAPO-Soundboard/releases/new
2. Choose a **new tag** `scratch-v1.0.0`, targeting the **current head of `experimental/scratch-freeze`**, which includes the final-status documentation. Do not target `main`, the v0.9.8 tag or a previous v1.0.0 source commit.
3. Title: `Scratch Edition v1.0.0 — Final Release`
4. Attach **`APOSoundboard-ScratchEdition-v1.0.0-x64.zip`**, the matched DLL/controller package already tested on Windows. Do not substitute GitHub's automatic Source code ZIP.
5. Uncheck **Set as a pre-release** and check **Set as the latest release** if you want the final high-fidelity edition to be the repository-wide Latest. GitHub supports only one latest release per repository; the lightweight edition remains separately available at `v0.9`.
6. Paste the description below and select **Publish release**.

## Ready-to-paste release description

### Final Scratch Edition — high-fidelity Windows x64 soundboard

APO Soundboard Scratch Edition v1.0.0 is the completed high-fidelity soundboard for Equalizer APO's microphone capture path. Includes 16 configurable pads, global hotkeys, custom volume and Pitch, Auto Sweep, local monitoring and the interactive Scrub waveform.

**Scrub controls:** Click anywhere on a waveform to take control even while the selected pad is playing, drag backward/forward, hold to sustain, and release to pause at the exact position. Play/Pause, Stop and Spacebar (Soundboard → Scrub only) affect only the selected pad. The v0.9.10 fix for the repeated-drag buzz is included.

**Final v1.0.0 improvements:** Official waveform/red-cursor icon embedded in the controller EXE for Explorer, taskbar, title bar and Alt+Tab; Windows v1.0.0 metadata in EXE and DLL; improved monitor-device failure handling, clearer diagnostics, and UI status refreshes.

The approved full-spectrum audio DSP is unchanged from v0.9.10. The maintainer has confirmed the release works on their Windows setup, and the included ten offline test programs completed successfully (one optional audio test fixture was unavailable).

**Install:** Close older Scratch controllers; extract the entire ZIP, load `APOSoundboardScratch_v1.0.0.dll` on the Equalizer APO capture/microphone chain, and keep `APOSoundboardScratchController.exe` beside the DLL. Do not mix different Scratch DLL and controller versions.

**Separate edition:** Lightweight v0.9.0 remains available for low-end systems. Scratch Edition is the more resource-intensive high-fidelity version.

## Earlier releases

- **Published Scratch history:** `scratch-v0.9.1` through `scratch-v0.9.8` already exist with correct historical source tags and matching ZIP assets. Leave their tags and binaries unchanged.
- **v0.9.9:** an interim build introducing Scrub transport controls but containing a known audio crossfade regression. Do **not** publish it as a recommended download.
- **v0.9.10:** the interim hotfix was delivered as a local ZIP; it is superseded by v1.0.0. Do not create `scratch-v0.9.10` against v1.0.0 source, as that would be inaccurate.
- **Stable lightweight:** release tag `v0.9`; code on `main`. Preserve independently.

Full release history and short descriptions: [SCRATCH_RELEASES.md](SCRATCH_RELEASES.md).
