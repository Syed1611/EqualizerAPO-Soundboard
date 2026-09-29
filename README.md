# APO Soundboard

A lightweight **64-bit VST2 soundboard for Equalizer APO** that mixes triggered audio directly into the processed microphone stream. It is designed to work without VB-Cable, Voicemeeter, or another virtual audio device.

> Current recommended build: **v0.6.4**
>
> v0.3 and v0.4 are preserved as experimental history and are not recommended for normal use.

## Features

- 16 soundboard pads with global hotkeys
- Single-click to select a pad; double-click to play
- Selectable waveform region: drag to choose the exact part that should play
- Per-pad volume from 0% to 200%
- Per-pad pitch from -12 to +12 semitones
- 0.5-unit fine steps plus magnetic major snap points
- WAV decoding built in
- Additional formats through Windows Media Foundation, including common MP3/AAC/M4A/WMA/FLAC configurations supported by Windows
- Up to 32 overlapping voices
- Microphone passthrough + soundboard mixing inside the VST
- Hotkey conflict detection
- Built-in diagnostics for RAM, CPU, handles, threads, GDI/USER objects, VST connections and uptime
- Atomic config saving with backup/recovery
- No installer in current releases

## How it works

Equalizer APO hosts `APOSoundboard_v0.6.4.dll` in the microphone processing chain. A small companion controller owns the UI, loaded samples and global hotkeys. The controller sends soundboard sample/control data to the VST over a **local named pipe**. The actual mic + soundboard mix happens inside the Equalizer APO VST processing path.

The controller does **not** create a virtual microphone or virtual audio cable.

## Installation

1. Download the latest release ZIP.
2. Extract both files into the same folder, for example:

   `C:\Program Files\EqualizerAPO\VSTPlugins\APOSoundboard\`

3. In Equalizer APO Configurator, make sure Equalizer APO is enabled for the microphone/capture device you actually use.
4. In Configuration Editor, add **Plugins -> VST plugin** to that microphone chain.
5. Select `APOSoundboard_v0.6.4.dll`.
6. Click **Open panel**. The controller should launch.
7. Load sounds, assign hotkeys, and test with an application actively using the microphone.

Keep the DLL and `APOSoundboardController.exe` together because the VST launches the controller from its own directory.

## Pad controls

- **Single-click pad:** select it
- **Double-click pad:** play it
- **Waveform drag:** select the playback region
- **Waveform double-click / Full Range:** restore full-sample playback
- **Volume slider:** 0-200%, 0.5% fine steps, magnetic 25% notches
- **Volume slider double-click:** reset to 100%
- **Pitch slider:** -12 to +12 st, 0.5 st fine steps, magnetic 3 st notches
- **Pitch slider double-click:** reset to 0 st
- **Set Hotkey:** capture a global shortcut with conflict detection
- **Stop All:** stop active soundboard voices

Pitch is sampler-style resampling: changing pitch also changes playback speed/duration.

## Configuration

Current builds store settings under:

`%LOCALAPPDATA%\APOSoundboard\config_v05.bin`

A backup is kept alongside it as `config_v05.bin.bak`. Writes are staged through a temporary file and protected by a checksum so a truncated/corrupted config can be detected and recovered.

## Compatibility notes

- Windows x64
- Equalizer APO x64
- VST2 host path used by Equalizer APO
- Apps that bypass the Windows system-effects path, such as some ASIO or WASAPI-exclusive setups, may also bypass Equalizer APO and therefore this plugin.
- Binaries are currently unsigned, so Windows SmartScreen or antivirus software may warn about an unknown application.

## Building from source

The current source is intentionally small and can be cross-compiled with LLVM `clang` + `lld-link`. See [BUILDING.txt](BUILDING.txt) for the exact commands used for v0.6.4.

## Security / resource notes

The current code does not use networking, registry startup persistence, process injection, remote threads, or downloader logic. The UI uses Win32/GDI rather than a GPU rendering framework. The v0.6.1+ line also includes bounded decoded-sample memory and stricter worker-thread/COM/Media Foundation cleanup.

See [AUDIT.txt](AUDIT.txt) for the latest static-build checks.

## Version history

See [CHANGELOG.md](CHANGELOG.md). Historical release notes are also preserved in [`release-notes/`](release-notes/).

## License

MIT. See [LICENSE](LICENSE).

## Disclaimer

This is an independent project and is not affiliated with or endorsed by Equalizer APO.