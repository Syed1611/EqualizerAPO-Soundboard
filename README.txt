APO Soundboard v0.6 x64
=======================

This is a focused update to the working v0.5 / v0.2-style architecture.
No installer is included.

Keep these two files together:
  APOSoundboard_v0.6.dll
  APOSoundboardController.exe

In Equalizer APO, use APOSoundboard_v0.6.dll as the VST plugin.
Open Panel launches APOSoundboardController.exe.

Changes in v0.6
---------------
1. Correct pad click behavior
   - Single-click: select the pad only.
   - Double-click: play that pad.
   - Playback uses the highlighted waveform range.
   - Empty pads no longer open the file dialog just from a single-click;
     select the pad, then use Load / Replace Audio.

2. Volume drag control
   - Drag horizontally to set 0% to 200%.
   - The selected-pad text shows the exact percentage.
   - Double-click the Volume bar to reset to 100%.

3. Pitch drag control
   - Drag horizontally to set -24 to +24 semitones.
   - The selected-pad text shows the exact semitone value.
   - Double-click the Pitch bar to reset to 0 semitones.
   - Pitch remains sampler-style: changing pitch also changes playback speed.

4. Existing waveform + multi-format behavior retained
   - Drag over the waveform to select the playback range.
   - Double-click the waveform, or press Full Range, to restore the full sample.
   - WAV is built in; Windows Media Foundation is used for other supported
     formats such as MP3, FLAC, M4A/AAC and WMA where Windows can decode them.

Settings
--------
- v0.6 intentionally uses the same config_v05.bin format as v0.5 because no
  saved-data fields changed. Your v0.5 pad assignments, volume, pitch,
  waveform ranges and hotkeys should carry over directly.

Important
---------
This build was cross-compiled and statically checked as x86-64 Windows PE.
It cannot be runtime-tested inside Equalizer APO from the build environment.
Keep your known-working v0.5 files until you confirm v0.6 on your PC.