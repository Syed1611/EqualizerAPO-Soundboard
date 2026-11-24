# Changelog

## v1.1 — Scratch Edition

- Consolidate the Scratch development work into the v1.1 release; leave the lightweight edition separate.
- Restore the original R2 OpenPanel scrub path, including cursor handling, mono host-rate source conversion, FFT/WSOLA reconstruction, entry transition and release-to-pause.
- Improve non-neutral scrub pitch with 64-tap band-limited sinc interpolation. Preserve neutral pitch.
- Add Silent/Continuous hold behavior; grey Silent track, blue Continuous track, animated toggle. Align Select Range/Scrub with selected-pad info at default font sizes.
- Retain two pages of pads, per-pad effects, independent monitor/output gains with linking, 400% approval, live adjustable typography, tray and configuration handling.
- Preserve known-compatible protocol-v3 IPC handshake behavior and detailed mismatch diagnostics.

The source history and regression references are retained. No intermediate development revision is being published as part of v1.1.

