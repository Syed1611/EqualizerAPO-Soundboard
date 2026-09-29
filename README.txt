APO Soundboard v0.6.2 x64
========================

This keeps the working v0.6/v0.6.1 soundboard behavior and UI. No installer.
Keep APOSoundboard_v0.6.2.dll and APOSoundboardController.exe in the same folder.

New in v0.6.2
-------------
- Hotkey conflict warnings:
  * If another pad already uses the shortcut, the warning names that pad.
  * If Windows/another application rejects the shortcut, the previous hotkey is restored.
  * Saved hotkeys that cannot register are marked [CONFLICT] on the affected pad.
- Built-in diagnostics at the bottom of the controller, updated once per second:
  RAM, decoded-sample RAM, CPU, process handle count, thread count, GDI objects,
  USER objects, VST connections, hotkey conflicts, config-backup status and uptime.
- Automatic config backup/recovery:
  * Config writes go to a temporary file first and are replaced atomically.
  * The previous known-good config is kept as config_v05.bin.bak.
  * New saves include a CRC check.
  * If the main config is damaged/truncated, the backup is loaded automatically.

Existing v0.6.1 stability hardening is retained:
- Safe VST worker-thread shutdown.
- Clean Media Foundation / COM shutdown.
- Bounded decoded sample RAM: 128 MiB per pad, 512 MiB total.
- No cross-thread UI text updates.
- GDI resources and handles are explicitly cleaned up.

Configuration compatibility
---------------------------
The controller still uses %LOCALAPPDATA%\APOSoundboard\config_v05.bin and can read
existing v0.5/v0.6/v0.6.1 settings. v0.6.2 upgrades future saves to a checksummed
version of that same config file. The backup is stored alongside it as
config_v05.bin.bak.

Runtime note
------------
This build was cross-compiled and statically checked, but cannot be executed inside
Equalizer APO in the build environment. Keep your previous working build until you
have verified v0.6.2 on your Windows system.