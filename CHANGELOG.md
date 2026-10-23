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
