APO Soundboard v0.6.4 x64
========================

Focused control update based directly on the working v0.6.3 build. No installer.
Keep APOSoundboard_v0.6.4.dll and APOSoundboardController.exe in the same folder.

New in v0.6.4
-------------
- Pitch range is now -12.0 to +12.0 semitones.
- Fine adjustment remains 0.5 semitone per step.
- Volume fine adjustment remains 0.5 percentage point per step.
- Both sliders now have visible external major-notch marks and magnetic snapping.
- Volume major notches: 0, 25, 50, 75, 100, 125, 150, 175, 200%.
- Pitch major notches: -12, -9, -6, -3, 0, +3, +6, +9, +12 st.
- Dragging close to a major notch snaps onto it; values between notches remain available in 0.5 steps.
- Double-click reset remains: Volume = 100.0%, Pitch = 0.0 st.

Everything else from v0.6.3 is retained, including:
- Single-click pad = select; double-click pad = play.
- Selectable waveform playback range.
- WAV plus Windows Media Foundation audio decoding.
- Global hotkey conflict protection.
- Diagnostics panel.
- Config backup/recovery and stability hardening.

Configuration compatibility
---------------------------
The controller continues to use:
  %LOCALAPPDATA%\APOSoundboard\config_v05.bin

Existing settings are read directly. Pitch values outside the new +/-12 st range
are safely clamped to the nearest new limit when loaded. The config format and
backup mechanism are otherwise unchanged.

Runtime note
------------
This build was cross-compiled and statically checked, but cannot be executed
inside Equalizer APO in this build environment. Keep the previous working build
until v0.6.4 has been verified on your Windows system.