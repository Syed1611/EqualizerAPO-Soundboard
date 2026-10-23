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
