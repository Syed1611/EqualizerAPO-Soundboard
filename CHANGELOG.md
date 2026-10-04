# Changelog

All notable project changes are recorded here. Experimental and superseded releases are kept in the history instead of being silently removed.

## [0.8.7] - Current stable

### Fixed
- Diagnostics now fully replaces the Soundboard page instead of leaving stale pad, waveform or slider pixels underneath.
- Page visibility is enforced on every layout pass.
- Diagnostics uses 3 columns at normal desktop widths and 2 columns at narrower widths.
- Diagnostic cards are clamped to the visible client area.

### Retained
- v0.8.6 WinMM-compatible monitor path.
- Responsive Soundboard / Diagnostics organization.
- Pad right-click workflow and live audio-engine features.

## [0.8.6] - Superseded by v0.8.7

### Fixed
- Replaced the unreliable native tab dependency with always-visible Soundboard / Diagnostics selector buttons.
- Removed layout redraw suppression that could leave child controls invisible until clicked.
- Explicitly repaints visible native controls after layout.
- Keeps all 16 pads compact instead of stretching them with window height.
- Empty pads now instruct the user to right-click to load.

### Changed
- Restored the v0.8.1/v0.8.2 WinMM `waveOut` local-monitor path as the preferred backend.
- Restored compatible playback-device enumeration including Windows Default.
- Explicit-device open failure falls back to Windows Default.
- WASAPI remains a fallback path.

### Known issue
- Diagnostics could still visually overlap stale Soundboard controls on some systems; fixed in v0.8.7.

## [0.8.5] - Superseded

### Added
- Soundboard and Diagnostics views.
- Responsive/resizable UI concept.
- Pad right-click menu for Set/Clear Hotkey, audio management, rename, resets, file location, duplication and full pad reset.
- Tools menu with playback-device refresh and Monitor Test.
- Side-by-side grouped diagnostic cards.

### Changed
- Main Soundboard view was simplified around pads, waveform, monitor controls, live Volume/Pitch and pad naming.
- Config format added saved window size and selected view.

### Known issues
- Some native controls/tabs could fail to paint until clicked or be positioned incorrectly on real Windows systems.
- The monitor/WASAPI path remained unreliable on some devices.
- These issues led to the v0.8.6 compatibility rollback/hotfix.

## [0.8.4] - Superseded

### Changed
- Returned the controller layout to the v0.8.2-style fixed UI after v0.8.3 layout regressions.
- Retained non-UI monitor/audio optimizations from v0.8.3.
- Relaxed exact-build matching while retaining protocol-v3 compatibility checks.

### Known issue
- WASAPI monitor initialization still failed on some real playback-device configurations.

## [0.8.3] - Experimental / superseded

### Experimented with
- Event-driven WASAPI local monitoring.
- Dedicated multimedia-priority monitor thread.
- Circular monitor buffering.
- Clock-drift correction between capture and playback devices.
- Underrun/overrun recovery fades and monitor-specific diagnostics.
- Resizable/DPI-aware controller layout.

### Status
- The monitor/audio ideas were useful, but the new layout produced clipped/missing controls on real Windows systems. The UI changes were rolled back afterward.

## [0.8.2]

### Added
- OGG/OGA and Opus file extensions to the supported loading path through Windows Media Foundation.
- Remove Audio.
- Reset Pad.
- Reload Audio.
- Selected-file information and decoded-memory display.
- Monitor output-device selection.
- Independent local monitor volume.

### Changed
- Background decode/resample concurrency is bounded.
- Additional compressed-file validation and decoded-value sanitizing.
- Waveform generation is folded into decode/resample work where practical.
- UI painting was further optimized to reduce startup and random flicker.

### Notes
- OGG/Opus decoding still depends on a compatible Windows Media Foundation decoder; v0.8.2 did not include a bundled native Vorbis/Opus decoder.

## [0.8.1]

### Added
- Local soundboard monitoring to the Windows playback device.
- Monitor ON/OFF control.
- Monitor-specific drop/error telemetry.

### Changed
- Waveform/playhead and custom sliders use persistent GDI backbuffers.
- Playhead refresh rate was reduced to lower repaint pressure.
- Host-rate telemetry is applied to the live controller/monitor path when the APO host rate changes.

## [0.8.0]

### Added
- True real-time per-pad pitch affecting voices already playing.
- True real-time per-pad volume.
- Parameter smoothing for live pitch/volume changes.
- Live waveform playback playhead.
- Background sample decoding.
- Per-pad Loading / Missing / Decode Error states.
- Host-rate sample conversion.
- Missing-file relocation.
- Oldest-voice stealing when all 32 voices are occupied.
- Board Mute toggle and global Board Mute hotkey.
- Audio Engine Self Test.
- Controller/VST protocol v3 handshake and mismatch reporting.
- Automatic reconnect handling and mic-only VST fallback when the controller disconnects.
- Expanded performance/session diagnostics and Reset Statistics.

### Performance
- Lightweight atomic reads for live parameters.
- Preallocated active voice/sample structures.
- Deferred sample reclamation outside the mixer hot path.
- Pipe request sizing follows the APO host block size.
- Idle/deep-idle work reduction.
- Denormal protection.
- LLVM `-O3` optimization/vectorization where appropriate.

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
- Hotkey capture temporarily unregisters soundboard hotkeys while listening for a new shortcut.
- Trying to reuse an existing shortcut no longer plays the pad already bound to it.
- Conflicting assignments are rejected immediately and existing hotkeys are restored after capture/cancel.
- `WM_HOTKEY` events are ignored while capture mode is active.

### Changed
- Volume uses 0.5% steps.
- Pitch uses 0.5-semitone steps.
- Selected-pad values display decimal half-steps.

## v0.6.2

### Added
- Hotkey conflict warnings for pad-to-pad conflicts and shortcuts rejected by Windows/another application.
- `[CONFLICT]` indication on affected saved pads.
- Built-in diagnostics strip.
- Automatic config backup/recovery with staged writes and CRC validation.

### Stability
- Retained v0.6.1 resource/thread hardening.

## v0.6.1

### Fixed / hardened
- VST worker thread must stop before its owning instance can be freed.
- Media Foundation and COM shut down cleanly.
- Single-instance mutex closes cleanly.
- Worker threads no longer update GUI text directly across threads.
- Decoded sample RAM is capped at 128 MiB per pad and 512 MiB total.
- Reduced idle pipe polling wakeups.

### Audit notes
- No networking APIs, startup persistence, process injection, downloader logic or GPU rendering path.

## v0.6.0

### Fixed
- Single-click selects a pad only.
- Double-click plays the selected pad.
- Empty pads no longer open the file dialog from a normal single click.

### Changed
- Replaced Volume +/- buttons with a horizontal drag control; double-click resets to 100%.
- Replaced Pitch +/- buttons with a horizontal drag control; double-click resets to 0 st.
- Waveform-region and multi-format behavior from v0.5 retained.

## v0.5.0

### Added
- Returned to the known-working v0.2 VST/controller architecture.
- Per-pad pitch control.
- Selectable waveform with saved per-pad playback range.
- Playback/hotkeys honor the selected waveform range.
- Windows Media Foundation decoding path for additional formats beyond WAV.

### Compatibility
- Imports the old v0.2 `config.bin` and stores extended settings separately in `config_v05.bin`.

## v0.4.0 - Experimental / superseded

### Experimented with
- Custom sampler-style controller UI.
- Single-click select / double-click play interaction.
- Volume and pitch controls.
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
- Open Panel launches the controller safely.
- Installer no longer launches the controller immediately after installation.
- Removed the v0.1 startup behavior.

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
