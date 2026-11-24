# APO Soundboard — Scratch Edition v1.1

A Windows x64 VST2 soundboard for Equalizer APO's microphone capture chain. It injects triggered audio into the processed microphone stream without requiring a virtual audio cable. The controller and VST communicate over a local named pipe.

The lightweight edition remains separate on [`main`](https://github.com/Syed1611/EqualizerAPO-Soundboard/tree/main). For Windows binaries, see [GitHub Releases](https://github.com/Syed1611/EqualizerAPO-Soundboard/releases). Use the Scratch Edition v1.1 package when published; older releases are not interchangeable with the lightweight edition.

## Features

- 32 customizable pads across two pages, global hotkeys, selected-pad playback, Stop All and board mute.
- Select Range and Scrub modes, forward/reverse scrubbing, Silent and Continuous stationary behavior, and pitch adjustment.
- Original R2 OpenPanel FFT/WSOLA reconstruction, with band-limited 64-tap sinc pitch interpolation at non-neutral pitch. Neutral-pitch reconstruction follows the original path.
- Per-pad output volume, independent local monitoring, optional linked gains and extended volume up to 400% after approval.
- Per-pad reverb, echo, effect presets and Auto Sweep; stereo ordinary playback.
- Light/dark themes, adjustable typography, system tray, configuration backup/recovery and diagnostic information.

## Installation

1. Completely exit the old controller (including the tray process) and back up the Scratch configuration.
2. Extract the entire v1.1 ZIP into a new folder accessible to Equalizer APO.
3. In Equalizer APO Configuration Editor, select the microphone/capture chain. Replace the old VST entry with `APOSoundboardScratch_v1.1.dll` and apply the configuration.
4. Keep `APOSoundboardScratchController.exe` beside the DLL. Use Open Panel or start the controller directly.
5. Test pad playback, microphone output, local monitor and hotkeys before relying on the new build. Restart Windows if the audio engine is still loading the previous DLL.

Use the matching v1.1 DLL and controller. The controller also accepts known-compatible protocol-v3 Scratch builds 100218–100220 to avoid false protocol mismatches; this is not permission to mix unrelated releases. Do not run multiple controllers.

## Scrubbing and controls

In Scrub mode, mouse-down takes over the selected pad. Move left/right for reverse/forward scrubbing; the pitch control is independent of cursor motion. In **Silent**, the dry scrub signal fades out after movement stops; in **Continuous**, the engine sustains audio while holding. Existing effect and output-buffer tails can remain after motion stops. The switch is grey on the left, blue on the right and disabled in Select Range mode. Release pauses at the cursor; Play or Space (when focused on Scrub, outside text/numeric input) resumes. Stop resets the selected pad.

Scrub-associated playback uses the original R2 mono reconstruction and source conversion. Normal pad playback retains stereo. The original reconstruction can include nearby source audio and may produce artifacts at extreme stretch ratios. No claim of lossless arbitrary stretching or dedicated formant correction is made.

Output volume is per pad. When **Link volumes** is off, monitor volume is an independent global master; when enabled, each playing pad uses its own output level for both paths. Selecting a different pad changes the displayed controls without changing other pads' audio. Both gain paths support extended volume after the shared warning is approved; the limiter can reduce clipping but does not eliminate excessive loudness. Use a safe listening level.

Tools → Appearance → Font sizes (live) provides six independently adjustable 8–20 pt settings. The default font is Segoe UI Variable, with 11 pt general text, 10 pt pad names/hotkeys and dropdowns, 12 pt values, 14 pt dialog headings and 10 pt dialog text. Native Windows font rendering and DPI behavior depend on the system.

## Build and testing

Source is freestanding C, built with Clang and LLD. See [BUILDING.txt](BUILDING.txt), [TESTING.txt](TESTING.txt) and [VALIDATION.md](VALIDATION.md). Internal build 110000, Windows file version 1.1.0.0, IPC protocol v3. Configuration remains `config_scratch_v12.bin`.

Native Windows GUI, Equalizer APO runtime and comparative listening need local verification. The stock Equalizer APO Open Panel intermediate dialog is unchanged; the optional host patch in `host-integration/` is not built or bundled as a host replacement. The binaries are unsigned.

The release ZIP includes a SHA-256 manifest for its packaged files; the source branch omits the built binaries. See [CHANGELOG.md](CHANGELOG.md) and [RELEASING.md](RELEASING.md). MIT licensed; independent of Equalizer APO. This project uses both hand-written code and AI assistance for prototyping, debugging and testing.
