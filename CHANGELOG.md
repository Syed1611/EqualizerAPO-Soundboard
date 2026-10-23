# APO Soundboard — Scratch Edition v0.9.6

## Experimental: Unified Spectral Scrubbing

- Replaced the moving-grain/WSOLA renderer with continuous spectral-peak
  tracking and additive resynthesis. No 78ms recorded-frame hopping or
  11ms moving-fragment seam blending remains.
- Re-analysis occurs only when the waveform pointer position changes;
  holding the cursor does not trigger new source segments.
- Phase continuity retained while spectral content follows forward or
  reverse cursor movement. The original pitch, user Pitch control and
  Auto Sweep behavior are preserved independently of cursor speed.
- Removed an overlapping board-mix soft saturator; introduced a single
  dynamic linear VST output gain limiter for mic + soundboard and separate
  monitor peak protection before converting to PCM16.
- Diagnostics now label pre-limiter over-level input correctly; these
  counters do not measure clipping after the limiter.
- Removed mouse timestamp-based 90ms freeze switching.
- Experimental v5 pipe isolates this engine from previous Scratch releases.
- Added offline regression for hold, forward, reverse, live pitch, large
  motion jumps, finite samples and peaks.

### Limitations

A spectral oscillator bank smooths timbral changes but cannot exactly
reproduce all transients and noisy vocal components. This is an experimental
quality revision, not proof of perfect real-time time stretching. Windows
Equalizer APO listening and monitoring tests remain necessary. Stable
v0.9.0 on GitHub was not changed.
