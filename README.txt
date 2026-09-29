APO Soundboard v0.1.0 (x64)
============================

Purpose
-------
APO Soundboard is a small 64-bit VST2 effect made specifically for Equalizer APO.
It mixes soundboard audio into the same capture stream as your microphone, so apps
that receive the Equalizer APO-processed microphone hear both your mic and the pad.
It does NOT create or require VB-Cable, Voicemeeter, or another virtual audio device.

Package
-------
APOSoundboard.dll               - 64-bit VST2 effect loaded by Equalizer APO
APOSoundboardController.exe     - 16-pad soundboard + global hotkeys
Install APOSoundboard.cmd       - installs both files and starts controller at login
Uninstall APOSoundboard.cmd     - removes installed files/startup entry
src\                            - complete C source
BUILDING.txt                    - reproducible clang/lld build commands
LICENSE.txt                     - source license
SHA256SUMS.txt                  - file hashes

Why there is a controller EXE
-----------------------------
Equalizer APO runs the real audio-processing VST instance separately from the instance
used by Configuration Editor. Global hotkeys and user-selected sound files are therefore
handled by the controller. The controller sends only soundboard sample data/control over
a local Windows named pipe. The final mixing still happens inside APOSoundboard.dll in
the Equalizer APO microphone processing chain. This is not a virtual audio cable.

Install and configure
---------------------
1. Extract the whole ZIP to a normal folder.
2. Run "Install APOSoundboard.cmd". Approve the Administrator prompt.
3. Open Equalizer APO Configurator/Device Selector and make sure the actual microphone
   or capture device you use is enabled for Equalizer APO. Reboot if Equalizer APO asks.
4. Open Equalizer APO Configuration Editor.
5. If your config affects more than one device, add/select the appropriate Device filter
   so this VST is applied to the microphone/capture device.
6. Add: Plugins -> VST plugin.
7. Browse to:
     <Equalizer APO install folder>\VSTPlugins\APOSoundboard\APOSoundboard.dll
8. Launch APOSoundboardController.exe if it is not already open. The installer also adds
   it to your per-user startup list.
9. In the controller, click an empty pad to choose a WAV file. Select the pad and use
   "Set Hotkey" to capture a global shortcut. Press the shortcut from any app to play it.
10. Test with Windows Sound Recorder, Discord, a browser call, or another application
    that uses the normal Windows shared capture path.

Controller controls
-------------------
- 16 pads in a 4 x 4 grid.
- Click an empty pad: load/replace its WAV file.
- Click a loaded pad: play it immediately.
- Set Hotkey: press a key or key combination for the selected pad.
- Volume - / Volume +: changes the selected pad in 10% steps (0% to 200%).
- Stop All: stops all currently playing pad voices.
- Multiple pads can overlap; up to 32 active voices are mixed.
- Pad paths, hotkeys and volumes are saved in:
    %LOCALAPPDATA%\APOSoundboard\config.bin

VST parameters
--------------
Board   0-200%  Master level for soundboard audio. Default 100%.
Mic     0-200%  Microphone passthrough level. Default 100%.
Limiter Off/On  Hard-clamps the final stream to avoid values outside -1..+1. Default On.

Audio file support in v0.1
--------------------------
WAV only. Supported WAV sample formats:
- PCM 8-bit
- PCM 16-bit
- PCM 24-bit
- PCM 32-bit integer
- IEEE float 32-bit
- WAVE_FORMAT_EXTENSIBLE when its subtype is PCM or float

The controller resamples pad audio to the sample rate requested by Equalizer APO and
mixes stereo/multichannel WAV input down to a centered soundboard signal.

Important limitations
---------------------
- This is the first test build. The x64 PE binaries and VST2 export were statically
  checked, but this build environment cannot run Windows/Equalizer APO, so runtime
  validation must happen on your PC.
- The binaries are unsigned. Windows SmartScreen or antivirus may warn about a new,
  unknown executable. Complete source code and build commands are included for audit.
- Apps that bypass the Windows system-effects path (for example some ASIO or WASAPI
  exclusive-mode configurations) will also bypass Equalizer APO and this plugin.
- v0.1 intentionally focuses on reliable mic mixing + hotkeys. It does not yet include
  MP3/FLAC decoding, waveform trimming, per-pad looping/fades, drag/drop or profiles.

Troubleshooting
---------------
Controller says "VST audio connections: 0"
- Start an app that is actively using the microphone. Some capture streams do not run
  until an app is recording or is in a call.
- Confirm the microphone is checked/enabled in Equalizer APO Configurator.
- Confirm APOSoundboard.dll is in the active microphone's Equalizer APO config.
- If Equalizer APO itself is not affecting the mic, fix that first (Configurator install
  mode/driver compatibility, then restart Windows Audio or reboot).

Mic works but pad is silent
- Check the controller shows at least one VST audio connection.
- Click the pad directly to rule out a hotkey conflict.
- Try a standard 16-bit or 24-bit PCM WAV first.
- Check the VST Board level is not at 0%.

Mic becomes too loud/clips
- Lower Mic and/or Board in Equalizer APO's VST parameter controls.
- Keep Limiter On while testing.

A hotkey does not register
- Windows or another application may already own that global hotkey. Choose another
  combination such as Ctrl+Alt+F1 or Ctrl+Shift+Num1.

Security/architecture note
--------------------------
The local named pipe rejects remote/network clients. It exists only to carry pad audio
between the user-session controller and the VST instance; it does not expose an audio
capture/playback device to Windows applications.