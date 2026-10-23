# APO Soundboard — Scratch Edition v0.9.8

High-fidelity experimental/alternate Windows x64 VST2 soundboard for the microphone path in Equalizer APO. **This is the feature-rich alternative to [lightweight v0.9.0](https://github.com/Syed1611/EqualizerAPO-Soundboard/tree/main), not a replacement for it.**

This version contains the accepted v0.9.8 UI release candidate's **unchanged binaries and audio DSP**. The build has been exercised by the project owner on Windows; testing on other machines/configurations is still recommended.

## Features

- All the baseline 16-pad soundboard features: global hotkeys, custom names, volume and Pitch, Auto Sweep, waveform, diagnostics, light/dark themes, and optional local monitor.
- **Select Range mode:** drag the waveform to select a playback region; double-click resets the range. The blue range shading is visible only in this mode.
- **Scratch mode:** drag right to move forward, left to reverse; cursor motion determines effective traversal rate while the Pitch slider (and Auto Sweep) adjusts pitch independently. Holding the cursor maintains a continuous, spectrally synthesized sound. Releasing resumes playback.
- **High-fidelity full-spectrum DSP:** 2048-point FFT overlap-add synthesis retains spectral noise/detail rather than only selected peaks; near-natural-speed, neutral-Pitch playback may use decoded PCM directly.
- **UI refinements in v0.9.8:** Windows-style Soundboard/Diagnostics tabs; immediate Light/Dark refresh of information strips; responsive diagnostics cards without independently repainting Win32 group boxes; improved resize and tab-switch layout; renamed Scratch mode; bold centered Speed/Depth numeric boxes.
- **Separate edition:** version-specific Scratch named pipe, VST identity and controller EXE; independent pad config from stable v0.9.0.

## Installation

1. Close earlier **Scratch Edition** controllers before upgrading.
2. Unzip the **entire package** into its own directory, for example under `C:\Program Files\EqualizerAPO\VSTPlugins\APOSoundboardScratch\`.
3. In Equalizer APO Configuration Editor (microphone/capture chain), choose **Plugins → VST plugin** and load `APOSoundboardScratch_v0.9.8.dll`.
4. Keep `APOSoundboardScratchController.exe` next to the DLL; click **Open panel**.
5. Choose **Scratch** above the waveform and drag an audio clip while it is playing. Use **Select Range** for ordinary playback-region editing.

Do **not** mix a DLL from one Scratch build with a controller from another. Stable v0.9.0 remains available separately on `main`.

## Audio behavior and limitations

Moving the mouse changes source traversal and direction, not the selected pitch. Pitch and per-pad Auto Sweep remain live, even during stationary hold. Microphone mixing happens in Equalizer APO's VST; local output monitoring is optional. The full-spectrum scratch is not bit-identical to the original recording at arbitrary speeds or on freeze. Noise, consonants and difficult transients may sound synthetic. Some compressed formats require Windows Media Foundation codecs; no native Vorbis/Opus library is bundled.

## Building and verification

See [BUILDING.txt](BUILDING.txt), [TESTING.txt](TESTING.txt), [AUDIT.txt](AUDIT.txt) and [CHANGELOG.md](CHANGELOG.md). The release archive contains full C source and eight offline DSP tests. Both x64 binaries were compiled with clang/lld-link, the VST entry point was verified and static analysis found no issues. Those checks **do not replace** Windows/Equalizer APO runtime tests.

## Release channel

- **Lightweight / stable:** [main](https://github.com/Syed1611/EqualizerAPO-Soundboard/tree/main), v0.9.0.
- **Scratch Edition:** [experimental/scratch-freeze](https://github.com/Syed1611/EqualizerAPO-Soundboard/tree/experimental/scratch-freeze), v0.9.8.

## License

MIT — see [LICENSE](LICENSE). Not affiliated with Equalizer APO.
