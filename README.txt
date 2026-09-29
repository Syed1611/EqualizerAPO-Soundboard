APO Soundboard v0.5 x64 - test build
====================================

This build is based directly on the working v0.2 architecture.
There is no installer. Keep these two files in the same folder:

  APOSoundboard_v0.5.dll
  APOSoundboardController.exe

In Equalizer APO, add APOSoundboard_v0.5.dll as the VST plugin.
Open Panel launches APOSoundboardController.exe, just like v0.2.

Changes from v0.2
-----------------
1. Pitch
   - Pitch - / Pitch + buttons change the selected pad by 1 semitone.
   - Range: -24 to +24 semitones.
   - This is sampler-style pitch, so pitch also changes playback speed/length.

2. Selectable waveform playback
   - The selected pad's waveform is shown below the pad grid.
   - Drag across the waveform to highlight the playback region.
   - Pad clicks and global hotkeys play only the highlighted region.
   - Double-click the waveform, or click Full Range, to restore the full sample.
   - The selected region is saved per pad.

3. More audio formats
   - WAV decoding remains built in.
   - The controller also tries Windows Media Foundation for other formats.
   - The file picker includes: WAV, MP3, FLAC, M4A, AAC, and WMA.
   - Actual non-WAV format availability depends on the Media Foundation codecs
     installed in Windows. MP3/AAC/WMA are normally available on standard
     Windows 10/11 installs; FLAC support depends on the Windows installation.

Compatibility / settings
------------------------
- Existing v0.2 config.bin settings are imported automatically on first run.
- v0.5 saves its extended settings separately as config_v05.bin, so the old
  v0.2 config file is not overwritten.
- The VST/controller named-pipe architecture and microphone mixing path are
  the same design as v0.2.

Important
---------
This binary was cross-compiled and statically checked as x86-64 Windows PE.
It cannot be runtime-tested inside Equalizer APO from the build environment.
Treat it as a test build and keep your known-working v0.2 files available.