# Changelog

All notable project changes are recorded here. Versions v0.3.0 and v0.4.0 were experimental branches that were later superseded; they are retained for transparency and reproducibility.

## [0.7.0]

### Added
- HQ cubic pitch resampling.
- Adaptive VST audio buffering.
- Underrun counter and expanded host/audio diagnostics.
- Custom pad names.
- Active playing indicators for pads.
- Global Stop All hotkey.
- Live output peak and clipping indicator.

### Changed
- Controller/VST communication protocol upgraded to v2.
- Pitch ratios are precomputed.
- Mixer bypasses unnecessary processing while idle.
- Existing v0.6.x configs are upgraded automatically when saved.

### Performance
- Reduced unnecessary locking while no voices are active.
- Reduced idle mixer work.
- Adaptive buffering reacts to underruns while maintaining a bounded latency target.

## v0.6.4

### Changed
- Reduced per-pad pitch range from +/-24 st to **-12.0 to +12.0 semitones**.
- Kept fine pitch adjustment at **0.5 st** and volume adjustment at **0.5 percentage point**.
- Added visible external major tick marks to both sliders.
- Added magnetic snap points:
  - Volume: 0, 25, 50, 75, 100, 125, 150, 175, 200%.
  - Pitch: -12, -9, -6, -3, 0, +3, +6, +9, +12 st.
- Existing saved pitch values outside +/-12 st are clamped on load.

## v0.6.3

### Fixed
- Hotkey capture now temporarily unregisters all soundboard hotkeys while listening for a new shortcut.
- Trying to reuse an existing shortcut no longer plays the pad already bound to it.
- Conflicting assignments are rejected immediately and existing hotkeys are restored after capture/cancel.
- `WM_HOTKEY` events are ignored while capture mode is active.

### Changed
- Volume now uses 0.5% steps.
- Pitch now uses 0.5-semitone steps.
- Selected-pad values display decimal half-steps.

## v0.6.2

### Added
- Hotkey conflict warnings for both pad-to-pad conflicts and shortcuts rejected by Windows/another application.
- `[CONFLICT]` indication on affected saved pads.
- Built-in diagnostics strip showing RAM, decoded-sample RAM, CPU, handles, threads, GDI/USER objects, VST connections, conflict count, backup state and uptime.
- Automatic config backup/recovery with staged writes and CRC validation.

### Stability
- Retained v0.6.1 resource/thread hardening.

## v0.6.1

### Fixed / hardened
- VST worker thread must stop before its owning instance can be freed, removing a use-after-free risk during APO reload/reconfiguration.
- Media Foundation and COM are shut down cleanly.
- Single-instance mutex is closed cleanly.
- Worker threads no longer update GUI text directly across threads.
- Decoded sample RAM is capped at 128 MiB per pad and 512 MiB total.
- Reduced idle pipe polling wakeups.

### Audit notes
- No networking APIs, startup persistence, process injection, downloader logic or GPU rendering path.

## v0.6.0

### Fixed
- Single-click now selects a pad only.
- Double-click now plays the selected pad.
- Empty pads no longer open the file dialog from a normal single click.

### Changed
- Replaced volume +/- buttons with a horizontal drag control; double-click resets to 100%.
- Replaced pitch +/- buttons with a horizontal drag control; double-click resets to 0 st.
- Waveform-region and multi-format behavior from v0.5 retained.

## v0.5.0

### Added
- Returned to the known-working v0.2 VST/controller architecture.
- Per-pad pitch control, initially -24 to +24 semitones.
- Selectable waveform with saved per-pad playback range.
- Playback/hotkeys honor the selected waveform range.
- Windows Media Foundation decoding path for additional formats beyond WAV.

### Compatibility
- Imports the old v0.2 `config.bin` and stores extended settings separately in `config_v05.bin`.

## v0.4.0 - Experimental / superseded

### Experimented with
- Custom sampler-style controller UI.
- Single-click select / double-click play interaction.
- Volume and pitch knobs.
- Selectable waveform regions.
- More audio formats through Media Foundation.
- Minimize-to-tray behavior.
- Persistent pipe thread and precomputed waveform peaks.

### Status
- This branch did not prove reliable in runtime testing and was discarded. v0.5 restarted from the working v0.2 architecture.

## v0.3.0 - Experimental / superseded

### Experimented with
- A single-DLL architecture intended to remove the companion controller executable.
- Internal controller launch through Windows rather than shipping an installer/helper executable.
- Embedded custom UI and pitch controls.

### Status
- Equalizer APO did not expose the intended usable UI reliably. The approach was abandoned and the project returned to the two-file v0.2 architecture.

## v0.2.0

### Fixed
- Implemented the VST editor dispatcher path expected by Equalizer APO, fixing the v0.1 **Open panel** crash.
- Open Panel now launches the controller safely.
- Installer no longer launches the controller immediately after installation.
- Removed the v0.1 startup behavior.
- Changed the DLL filename so v0.1 and v0.2 can coexist during upgrade/testing.

## v0.1.0

### Added
- First 64-bit VST2 soundboard prototype for Equalizer APO.
- 16 pads and global hotkeys.
- WAV loading.
- Per-pad volume.
- Up to 32 overlapping voices.
- Microphone passthrough plus soundboard mixing.
- Board/Mic master levels and limiter.
- Local named-pipe controller-to-VST bridge.
- Reproducible source/build files and MIT license.
