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
