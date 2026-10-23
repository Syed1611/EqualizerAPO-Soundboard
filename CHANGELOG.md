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
