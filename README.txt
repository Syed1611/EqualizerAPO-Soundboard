APO Soundboard v0.4 x64 - test build
====================================

NO INSTALLER.
Keep these two files together in the same folder:
  APOSoundboard_v0.4.dll
  APOSoundboardController_v0.4.exe

Recommended location:
  C:\Program Files\EqualizerAPO\VSTPlugins\APOSoundboard\

Equalizer APO setup
-------------------
1. Remove/disable the old soundboard VST entry while testing v0.4.
2. Add APOSoundboard_v0.4.dll as the VST plugin on your microphone capture chain.
3. Click Equalizer APO's Open panel button.
4. The small Equalizer APO host panel opens and launches APOSoundboardController_v0.4.exe.
5. Keep the DLL and EXE next to each other; the DLL finds the controller by filename.

Pad interaction
---------------
- Single click a pad: select only. It does NOT play.
- Double click a loaded pad: play it.
- Double click an empty pad: choose an audio file.
- Space: play the selected pad.
- LOAD: replace/load the selected pad.
- PLAY: play the selected pad.
- STOP ALL: stop every soundboard voice.
- SET HOTKEY: press a global key combination for the selected pad.

Knobs
-----
- Drag vertically on VOLUME: 0% to 200%.
- Drag vertically on PITCH: -24 to +24 semitones.
- Mouse wheel over a knob: fine adjustment.
- Double click VOLUME: reset to 100%.
- Double click PITCH: reset to 0 semitones.

Waveform / region playback
--------------------------
- The selected pad's waveform is shown below the pads.
- Drag across the waveform to highlight the region you want.
- Pad double-click, PLAY and global hotkey all play ONLY that highlighted region.
- Double click the waveform to reset to the full sample.
- Region selection is saved per pad.

Audio formats
-------------
- PCM/float WAV is decoded directly by the controller.
- Other files are decoded through Windows Media Foundation when Windows has a decoder for them.
- MP3 and common Windows media audio formats should work on normal Windows 10/11 installs.
- FLAC/AAC/M4A/WMA and OGG/Opus depend on the codecs/features available on that Windows installation.

Minimize to tray
----------------
- Minimize the controller and it hides to the notification area instead of staying on the taskbar.
- Double click its tray icon to restore it.
- Closing the window exits the controller and unregisters its global hotkeys.

v0.2 migration / rollback
-------------------------
- v0.4 can import the old v0.2 config.bin on first launch.
- v0.4 saves its new pitch/region settings in a separate config_v04.bin.
- The old v0.2 config is not overwritten, so rollback is safe.

Resource-use changes
--------------------
- One persistent named-pipe server thread instead of spawning a new worker thread for each reconnect.
- Fixed GDI resources created once and explicitly deleted on exit.
- Waveform peaks are precomputed when a sample loads rather than rescanning the full sample on every paint.
- Audio sample buffers are explicitly released on replacement and exit.

This build was cross-compiled and statically checked here, but cannot be runtime-tested inside Equalizer APO/Windows in this environment.