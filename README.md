# APO Soundboard

A lightweight **64-bit VST2 soundboard for Equalizer APO** that mixes triggered audio directly into the processed microphone stream. It is designed to work without VB-Cable, Voicemeeter, or another virtual audio device for microphone routing.

> Current recommended build: **v0.8.7**
>
> v0.3/v0.4 were experimental architecture branches. v0.8.3-v0.8.6 are preserved as part of the public development history but are superseded by v0.8.7.

## Features

- 16 soundboard pads with global hotkeys
- Single-click to select a pad
- Double-click to play a pad
- Right-click pad menu for pad-specific management
- Selectable waveform region with a live playback playhead
- Live per-pad volume while audio is already playing
- Live per-pad pitch from **-12 to +12 semitones**
- 0.5-unit fine steps with magnetic major snap points
- HQ cubic resampling and host-rate conversion
- Up to 32 overlapping voices with oldest-voice stealing
- Global Stop All and Board Mute hotkeys
- Custom pad names
- Background audio loading and bounded decoded-sample memory
- Missing-file relocation
- Local soundboard monitoring to a Windows playback device
- Responsive Soundboard / Diagnostics views
- Config backup, CRC validation and recovery
- Audio engine self-test and detailed runtime diagnostics
- No installer in current releases

## How it works

Equalizer APO hosts the versioned `APOSoundboard_vx.x.x.dll` in the microphone processing chain. A companion `APOSoundboardController.exe` owns the UI, loaded samples, global hotkeys, waveform regions, local monitoring, and persistent configuration.

The controller and VST communicate over a **local Windows named pipe**. The final microphone + soundboard mix happens inside the VST in Equalizer APO's capture path.

The controller does **not** create a virtual microphone or virtual audio cable.

## Installation

1. Download the latest release ZIP.
2. Extract the **entire APOSoundboard folder** into Equalizer APO's VST folder:

   `C:\Program Files\EqualizerAPO\VSTPlugins\`

3. Keep the versioned DLL and `APOSoundboardController.exe` together in that folder.
4. In Equalizer APO Configurator, make sure Equalizer APO is enabled for the microphone/capture device you actually use.
5. In Configuration Editor, add **Plugins -> VST plugin** to that microphone chain.
6. Select the APOSoundboard DLL from the extracted folder.
7. Click **Open panel** to launch the controller.
8. Load sounds, assign hotkeys, and test with an application actively using the microphone.

When upgrading, replace **both** the DLL and controller from the same release.

## Pad controls

- **Left click:** select pad
- **Double click:** play pad
- **Right click:** open pad actions
- **Waveform drag:** choose the playback region
- **Volume:** 0-200%, 0.5% steps, magnetic 25% snap points
- **Pitch:** -12 to +12 st, 0.5 st steps, magnetic 3 st snap points
- **Double-click Volume:** reset to 100%
- **Double-click Pitch:** reset to 0 st

Pitch is sampler-style resampling, so changing pitch also changes playback speed/duration.

### Pad right-click menu

- Set Hotkey...
- Clear Hotkey
- Load / Replace Audio...
- Reload Audio
- Remove Audio
- Rename Pad...
- Reset Volume to 100%
- Reset Pitch to 0
- Reset Playback Range
- Open File Location
- Duplicate Pad To...
- Reset Pad...

Duplicate Pad copies the pad's audio/settings but deliberately does **not** duplicate its hotkey.

## Audio formats

- **WAV:** built-in decoder for common PCM and float WAV formats
- **MP3, AAC/M4A, WMA, FLAC:** Windows Media Foundation
- **OGG / OGA / Opus:** accepted through Windows Media Foundation when Windows exposes a compatible Vorbis/Opus decoder

v0.8.7 does not bundle a native Vorbis/Opus decoder, so unusual OGG/Opus streams can still fail on some Windows installations.

## Local monitoring

The controller can play the soundboard locally while the same sound is mixed into the microphone path.

- Monitoring starts **OFF** on every launch
- Output device can be selected from the controller
- Monitor volume is independent from the microphone/soundboard mix level
- Headphones are recommended to avoid acoustic feedback into the microphone
- The current stable build prefers the compatibility-oriented Windows playback path used by the v0.8.1/v0.8.2 line, with fallback handling retained

## Diagnostics

The Diagnostics view groups runtime information into responsive side-by-side cards:

- **Audio Engine:** VST connection, sample rate, block size, ring fill, latency, underruns
- **Output:** peak, clipped frames, board state, active voices, voice steals, dropped triggers
- **Local Monitor:** device/backend, buffer, underruns/overruns, corrections, errors
- **System:** RAM, peak RAM, sample RAM, CPU, threads, handles, GDI/USER objects
- **Session:** reconnects, protocol mismatches, lowest ring fill, resampler/smoothing state, decode workers
- **Status:** hotkey conflicts, backup/config state, Stop All / Board Mute shortcuts, uptime

## Configuration

Settings remain under:

`%LOCALAPPDATA%\APOSoundboard\config_v05.bin`

The filename is retained for compatibility even though the internal config format has evolved. Current builds keep atomic saving, CRC validation and `.bak` recovery.

## Compatibility notes

- Windows x64
- Equalizer APO x64
- VST2 host path used by Equalizer APO
- Applications that bypass Windows system effects may also bypass Equalizer APO
- Binaries are currently unsigned, so SmartScreen/antivirus may warn about an unknown application

## Development

This project is roughly **half hand-coded by me and half vibe-coded with AI assistance**. AI is used to help prototype, debug, audit and iterate, while the project is tested and shaped around actual Equalizer APO use.

## Building from source

The current source can be cross-compiled with LLVM `clang` + `lld-link`. See [BUILDING.txt](BUILDING.txt) for the exact current build commands.

## Security / resource notes

The project does not use networking, downloader logic, registry startup persistence, process injection or remote-thread injection. Decoded sample memory is bounded, and the real-time path is designed to avoid unnecessary allocation/locking.

See [AUDIT.txt](AUDIT.txt) for current build checks.

## Version history

See [CHANGELOG.md](CHANGELOG.md).

## License

MIT. See [LICENSE](LICENSE).

## Disclaimer

This is an independent project and is not affiliated with or endorsed by Equalizer APO.
