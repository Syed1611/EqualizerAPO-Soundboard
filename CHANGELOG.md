# Experimental Scratch Edition changelog

## v0.9.8 — Diagnostics, Resize, Themes & Waveform Modes (accepted UI release)

- Replaced transparent Win32 BS_GROUPBOX diagnostics frames with complete
  parent-painted card borders/titles. This eliminates the overlapping redraw
  artifacts observed during simultaneous tab switching and window resizing.
- Constrained the diagnostic grid to the visible client bounds rather than
  growing card height beyond the window and clipping other cards.
- Added synchronous parent+child invalidation on WM_SIZE and forced a complete
  refresh of all currently displayed native text controls on theme toggle.
- Windows-style connected top tabs (Soundboard / Diagnostics); selected tab
  has no bottom border and uses the same color as the underlying page.
- Select Range and Scratch have separate waveform visuals: range shading
  appears only in Select Range; Scratch shows the red cursor without the range.
- Renamed 'Scratch & Freeze' to 'Scratch'; per-mode instructions/status text.
- Updated window title and build metadata to 0.9.8; isolated v7 IPC pipe.
- Kept the v0.9.7 full-spectrum scratch audio DSP bit-for-bit unchanged.

### Validation
- Windows x64 controller and VST compile/link; VSTPluginMain export present.
- Seven offline scratch-audio regression tests passed unchanged.
- Clang static analyzer produced no findings after cleanup.
- Layout geometry checked at narrow, medium, and wide sizes.
- Windows GUI was subsequently checked and accepted on the project owner's setup; cross-system validation is still recommended.

# Experimental Scratch Edition changelog

## v0.9.7 — Higher-fidelity hybrid scratch (experimental)

- **Full-spectrum phase-vocoder scratch**: replaces the 192-slot / 112-tracked-peak oscillator engine from v0.9.6 with complex FFT overlap-add synthesis using all 1025 nonredundant bins of a 2048-point real spectrum. Source PCM noise, timbre and transient detail are no longer discarded by peak filtering.
- **Independent live Pitch preserved**: pitch shifts the assembled spectrum through cubic time-domain resampling, rather than remapping sparse spectral bins. Auto Sweep continues to control Pitch.
- **Decoded PCM at natural speed**: an adaptive source read fades in near steady ±1× cursor movement when Pitch is neutral; special slowed/fast/frozen regions remain fully resynthesized.
- **Transitions and timing**: audio-clocked source-motion estimate informed by recent GUI velocity; controlled crossfades into/out of Scratch; waveform scratch selection, left/right direction and indefinite hold unchanged.
- **Separate instance**: pipe suffix `Scratch_v6`, v0.9.7 paired controller and DLL, existing experimental config compatibility. No stable v0.9.0 changes.
- **Quality regression**: added identity fidelity, 44.1k→48k conversion, broadband-noise, live Pitch, motion/reverse, and held cursor tests; offline real voice decoded and used in testing.

### Known limitations

- Holding a single point indefinitely still needs spectral synthesis and can sound artificial on consonants or noisy audio. Perfect original quality at arbitrary speed/freeze is not physically achievable.
- Actual Windows/Equalizer APO microphone and monitoring output was **not** runtime-tested; this is an experimental binary.

## v0.9.6 — Continuous spectral motion / limiter changes

- Single tracked-oscillator spectral engine for forward/reverse/hold, no old moving short-loop fragments.
- Headroom and monitoring/VST clipping protection.


---

# Scratch Edition v0.9.5 — Spectral Freeze & Clipping Fixes

## Replaced
- Removed the v0.9.4 held-audio short-loop system that repeatedly read the
  previous/next 45–92 ms of recording and crossfaded the seam.
- New frequency-domain spectral-peak synthesis. A single spectral analysis
  captures the tone near the cursor; oscillators keep their own continuous
  phases while the mouse is held still. The freeze never loops recorded audio.
- Refined frequencies using phase differences from two windowed FFT frames.

## Improved
- Smooth hold/move/hold transitions; live pitch and Auto Sweep still work.
- Source-level spectral soft knee; additional scratch board headroom.
- Smooth VST limiter instead of hard clipping on mixed output.
- Bounded peak selection and heap-resident FFT buffers to avoid oversized
  Windows worker-thread stack frames.

## Kept
- Direct Equalizer APO x64 microphone mixing; no virtual cables.
- Select Range and Scratch & Freeze waveform modes, reverse/fast scrubbing.
- Experimental DLL/controller/pipe/config remain separate from stable v0.9.0.

## Known limitations
- Consonants and noisy clips can still sound artificial.
- Dragging motion still uses the previous WSOLA-inspired segment engine.
- Compatible OGG/Opus codec may be needed on Windows.
- No Windows audio-hardware validation has been performed here.


---

# Scratch Edition v0.9.4 — Locked-Loop Freeze (Experimental)

## Fixed
- Eliminated repeated stationary position searches/re-anchoring that sounded
  like the held snippet was jumping one frame backward and forward.
- Stationary hold chooses one phase-matched forward-reading loop and keeps it.
- Short seam blend reduces boundary clicks without continuously blending
  different overlapping grains, preserving a clearer central sound.
- Corrected overlap accounting so the locked loop does not shift fundamental
  pitch (220 Hz stays ~220 Hz; +12 st ~440 Hz; -12 st ~110 Hz).
- Added hold→move→new hold regression coverage.

## Preserved
- Pitch and Auto Sweep still change scratch pitch live, even while holding.
- Mouse drag speed/direction do not automatically retune playback.
- Normal playback, Select Range, VST mic routing, optional monitor, controller
  UI and configuration behave as in v0.9.3.
- Stable GitHub v0.9.0 unchanged.

## Known limitations
- Audible repetition is inherent to a time-frozen segment, especially for
  transient-heavy speech/noise; perceptual quality needs Windows testing.
- Native OGG/Opus decoding is still not bundled.


---

# Scratch Edition v0.9.3 (Experimental)

## Fixed
- Pitch slider no longer gets ignored during Scratch & Freeze. Manual pitch
  and per-pad Auto Sweep apply live to scrubbed and held audio.
- Mouse speed and direction stay independent from pitch setting.
- Replaced continuously overlapped, phasey two-grain rendering with mostly
  direct single-segment playback and short, correlation-matched seam crossfades.
- Smooth pitch steps while scratching; entry blend uses selected pitch.
- Scratch Edition pipe upgraded from v2 to v3 for controller/VST isolation.

## Preserved
- Stable v0.9.0 remains unchanged.
- Existing pad hotkeys, samples, selectable waveform, monitor output, and
  experiment-specific configuration are retained.

## Known limitations
- Granular freezing is not lossless and can repeat transients unnaturally.
- Windows host and mic playback remain unverified in this build environment.


---

# Scratch Edition v0.9.2 (Experimental)

## Corrected
- Scratch changes playback direction and effective speed but **not pitch**.
- Eliminated the unintended 0.65x speed used when the mouse was motionless.
- Switched from two 58ms triangular grains with variable input read rate to
  96ms normalized, native-speed grains with overlap-add and waveform alignment.
- Enter/exit transitions now crossfade between actual scratch and normal audio
  instead of repeating a constant last sample during the release fade.
- New Scratch pipe instance prevents old v0.9.1 and new v0.9.2 mismatches.

## Preserved
- Scratch Edition remains separate from v0.9.0 stable.
- Existing pads, hotkeys, waveform Select Range, Auto Sweep, Diagnostics,
  local monitoring, pitch/volume sliders, and theme controls remain.

## Known limitations
- Strong reverse/fast mouse movements and audio transients can still produce
  grains or audible seams; held freezes intentionally repeat a segment.
- No real Windows/Equalizer APO test completed in the build environment.
- Ogg/Opus native codec support still absent.


---

# Scratch Edition v0.9.1 — Experimental

- Forked the controller/VST from the uploaded v0.9.0 release without changing it.
- Isolated VST unique ID, IPC named pipe, controller executable/instance,
  configuration file and window class from the stable edition.
- Added Select Range and Scratch & Freeze waveform modes.
- Scratch controls pitch, speed and direction via mouse movement.
- Holding the waveform motionless performs continuous two-grain audio freeze,
  rather than going silent; release crossfades back to ordinary playback.
- Scratching takes over an existing pad voice to avoid unmodified doubling.
- Bold, centered 16px Speed/Depth numeric fields.
- Existing waveform range, local monitor, live pitch, hotkeys and dark mode kept.

Note: experimental audio quality/latency may require tuning on Windows.
