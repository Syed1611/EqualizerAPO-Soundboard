APO Soundboard v0.6.3 x64
========================

This is a focused update to the working v0.6.2 build. No installer.
Keep APOSoundboard_v0.6.3.dll and APOSoundboardController.exe in the same folder.

New in v0.6.3
-------------
- Fixed hotkey capture conflict behavior:
  * All APOSoundboard global hotkeys are temporarily unregistered while Set Hotkey is listening.
  * Pressing a shortcut already assigned to another pad no longer triggers/plays that pad.
  * The attempted assignment is rejected immediately with a warning naming the conflicting pad.
  * Existing hotkeys are restored as soon as capture finishes or is cancelled.
  * WM_HOTKEY events are ignored while capture is active as an additional safety guard.
- Volume slider now snaps to 0.5% increments.
- Pitch slider now snaps to 0.5-semitone increments from -24.0 to +24.0 st.
- Double-click reset remains: Volume = 100.0%, Pitch = 0.0 st.
- Selected-pad readout now displays the decimal half-step values.

Configuration compatibility
---------------------------
The controller continues to use:
  %LOCALAPPDATA%\APOSoundboard\config_v05.bin

Existing v0.5/v0.6/v0.6.1/v0.6.2 settings are read directly. Older integer
pitch values are converted to equivalent half-semitone units when loaded.
New saves use config format version 4 with the same CRC/backup system.

All v0.6.2 diagnostics, automatic config backup/recovery, memory limits,
Media Foundation cleanup, and VST shutdown hardening are retained.

Runtime note
------------
This build was cross-compiled and statically checked, but cannot be executed
inside Equalizer APO in the build environment. Keep the previous working build
until you have verified v0.6.3 on your Windows system.