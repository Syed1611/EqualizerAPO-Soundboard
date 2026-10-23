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
