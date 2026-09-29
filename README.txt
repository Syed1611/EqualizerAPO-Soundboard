APO Soundboard v0.2.0 (x64)
============================

What changed from v0.1
----------------------
1. The installer does NOT launch the controller after installation.
2. The installer removes the old v0.1 Windows-startup entry, so the controller no longer pops up automatically at login.
3. Equalizer APO's "Open panel" path is implemented correctly. v0.1 omitted the VST editor dispatcher calls that Equalizer APO uses and could crash Configuration Editor.
4. Clicking "Open panel" now opens a safe small VST panel and launches APOSoundboardController.exe.
5. The v0.2 DLL uses a new filename (APOSoundboard_v0.2.dll) so it can be installed even if Windows still has the old v0.1 DLL loaded.

Upgrade from v0.1
-----------------
1. Close APOSoundboardController.exe if it is open.
2. Run "Install APOSoundboard.cmd" from this v0.2 package. Nothing should open after installation.
3. Open Equalizer APO Configuration Editor.
4. Remove/disable the old APOSoundboard.dll VST entry.
5. Add a VST plugin using:
     <Equalizer APO>\VSTPlugins\APOSoundboard\APOSoundboard_v0.2.dll
6. Click "Open panel". Configuration Editor should stay open; the controller should launch.
7. Keep the controller running while you want global hotkeys/soundboard playback. You may close the small VST panel.

Purpose
-------
APO Soundboard is a 64-bit VST2 effect made specifically for Equalizer APO. It mixes
soundboard audio into the same capture stream as your microphone. It does not create
or require VB-Cable, Voicemeeter, or another virtual audio device.

Files
-----
APOSoundboard_v0.2.dll          - 64-bit VST2 effect loaded by Equalizer APO
APOSoundboardController.exe     - 16-pad soundboard + global hotkeys
Install APOSoundboard.cmd       - installer (manual launch only; no controller auto-start)
Uninstall APOSoundboard.cmd     - removes v0.1/v0.2 installed files and old startup entry
src\                            - complete C source
BUILDING.txt                    - reproducible clang/lld build commands
SHA256SUMS.txt                  - hashes for packaged binaries/scripts

Controller
----------
- 16 pads in a 4 x 4 grid.
- Click an empty pad to load a WAV.
- Click a loaded pad to play it.
- Set Hotkey captures a global keyboard shortcut.
- Per-pad volume: 0% to 200%.
- Stop All stops all active soundboard voices.
- Up to 32 overlapping voices.
- Settings are stored in %LOCALAPPDATA%\APOSoundboard\config.bin.

VST parameters
--------------
Board   0-200%  Soundboard master level. Default 100%.
Mic     0-200%  Microphone passthrough level. Default 100%.
Limiter Off/On  Clamps the final stream to -1..+1. Default On.

Supported audio files
---------------------
WAV: PCM 8/16/24/32-bit integer, IEEE float 32-bit, and supported WAVE_FORMAT_EXTENSIBLE PCM/float files. The controller resamples to the capture stream sample rate.

If "Open panel" still causes a crash
------------------------------------
Make sure the VST entry points to APOSoundboard_v0.2.dll, not the old APOSoundboard.dll. If Configuration Editor cached the old DLL, close Configuration Editor completely and reopen it. If needed, reboot once so the old DLL is no longer loaded.

If controller shows "VST audio connections: 0"
----------------------------------------------
Start an application that is actively recording/using the microphone, confirm Equalizer APO is enabled for the actual capture device, and confirm the v0.2 VST is in that microphone's active configuration.

Runtime note
------------
The binaries are compiled as Windows PE32+ x86-64 and the DLL exports VSTPluginMain. This build environment cannot execute Equalizer APO on Windows, so your machine is the runtime validation environment.