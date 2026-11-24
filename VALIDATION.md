# v1.1 validation

44/44 offline C test programs passed on the v1.1 source (Clang -O1). Windows x64 EXE and VST DLL compiled; static version, manifest and host-launch checks passed. The controller comparison and pitch tests passed; neutral pitch held/forward/reverse traces remain sample-identical in the standalone test (288,000 samples). The controlled octave-up alias reduction is 76.686 dB; intended 8 kHz component retained to within 0.001 dB. The 12 controller traces check neutral-prefix equivalence and finite output after pitch changes. IPC mocked handshake includes the stable build and known-compatible older builds. See `validation/v1.1-*.txt`.

Native Windows visual/layout, device behavior, CPU headroom and comparative listening remain unverified. The grey/blue toggle and compact default-size layout are code changes, not native UI acceptance results; at larger general font sizes the full-width info row is retained. Scrub source overlap, formant processing, core DSP and all other audio processing are unchanged from the R21 base.

Historical validation records follow; older research audio demo WAVs are not distributed in v1.1.

# R21 validation

44/44 inherited offline C test programs passed against the final source (clang -O1). The new standalone pitch comparison and the 12 controller traces passed. Neutral pitch holds/forward/reverse: 288,000 samples identical to the original R2 core; four neutral controller trace prefixes identical, remaining traces checked for finite output. Pitch-shifted output is intentionally no longer bit-identical to R2. At 48 kHz, +12 semitones stationary two-tone fixture: alias at 16 kHz -12.239 to -88.925 dBFS, reduction 76.686 dB; desired 8 kHz tone within 0.001 dB. R19/R20/R21 IPC mock handshake and incompatible-peer rejection passed. Windows x64 PE binaries compiled, static checks passed. The original R2 header and effects header remain byte-identical to their references.

Native Windows runtime/listening and CPU headroom have not been verified. No claim of better neutral-pitch stretching, improved formants or reduced overlap. Non-neutral pitch filtering uses additional output-domain lookahead. See validation/r21-*.txt for current logs. All below is historical release validation.


**IPC hotfix 1:** See HOTFIX.txt for installation and validation. R19/R20 protocol-v3 handshake compatibility is restored; unknown builds remain rejected. The R20 audio pipeline is unchanged. The original 43-test full-suite results below are historical; the hotfix targeted tests are recorded in validation/r20-hotfix-*.txt and validation/r20-ipc-hotfix.txt.

# R20 validation

R20 restores the original v1.0.2 R2 OpenPanel scrub path, not merely the reconstruction header. The R2 FFT/WSOLA header is unchanged byte-for-byte; original cursor velocity, 55 ms speed timing, mono reconstruction, explicit initialization, 33 ms entry blend, original release behavior and cubic playback of the scrub-associated voice are restored. Releasing pauses at the cursor; Play or Space resumes. A separate cached R2 cubic host-rate mono buffer preserves original scrub input while normal playback keeps native stereo. Silent is an independent 55–90 ms activity deadline with 8/12 ms fades, and suppresses the dry entry blend; Continuous follows the R2 path. Rapid-click seek bookkeeping and double-click handling remain as compatibility safeguards. R19 effects and separate output/monitor mixer are retained. At unity gain, with effects disabled, 12 complete controller traces matched the original R2 output exactly (48 kHz and 44.1-to-48 kHz, three initial pitches, fresh and taken-over voices, moving/holding/reverse/release/resume). The original R2 source windows and neighboring-audio behavior are restored, not reduced. Scrub-associated voices stay mono after resuming until stopped; ordinary pad playback can still be stereo. If host-rate cache preparation fails or an active connection uses another rate, the renderer falls back to native PCM; cache preparation on first click may take time for very large files. Very sparse motion may interrupt Silent, and effects/pipe/device tails can remain after dry output stops. Native Windows interaction, live listening, and non-unity/effects equivalence to R2 are not verified.

43 offline C programs passed. Windows x64 cross-build and static checks passed; native Windows not available. Historical comparison logs refer to prior versions.

# R19 validation

The original R2 header was compared byte-for-byte against the v1.0.2-R2-experimental archive. `test_r19_restore.c` compares active mono renderer samples against the direct original core over forward motion, hold and reverse at 48 kHz; the maximum difference was zero in this fixture. It also verifies the 90 ms motion deadline cap and exact zero gain after the 12 ms fade. The current stereo side path uses a second original-core instance. Audio decoding, resampling and downstream mixing remain R18, so full-pipeline equality to R2 is not claimed. The old R15–R18 locality tests are retained as historical engine tests, not active-engine quality measurements. Updated motion tests reflect the deliberate shorter stop interval; sparse mouse events beyond the deadline may cause gaps. Native Windows appearance, end-to-end latency and listening were not tested. See validation/test-results.txt for final test coverage.

# R18 validation

See current logs under `validation/`. R18 checks actual cursor-update events with a simulated clock; Windows mouse dispatch, monitor/pipe buffering, and subjective quality remain unverified. The longer motion grace can delay silence by 160–300 ms plus a 12 ms fade and buffering. Events slower than the grace can still interrupt audio. Source locality measurements are weighted sample distances, not perceived audio leakage. No spectral/formant engine or hard cutoff introduced. Historical logs and comparisons below do not establish current listening quality.

# R17 validation

See `validation/test-results.txt` and `validation/r17-hold-test.txt` for current offline test logs. R17 modifies controller motion classification, dry gate, UI and stored option. The underlying R16 reconstruction and effects headers are unchanged. Native Windows interaction, appearance, icon antialiasing and comparative listening have not been verified. Silent mode leaves existing echo/reverb tails intact, and events spaced beyond the grace period can still cause intermittent silence. No pre-analysis or new formant-preservation engine was integrated.

## Historical R16 validation (for reference)

# R16 validation

- 38 offline C programs passed, including updated R16 locality comparison and transition test. Static Windows PE, version and launch checks passed. Cross-compiled Windows x64 controller and VST DLL; no Windows runtime was available.
- Baseline is the original R15 source preserved in tests/reference/r15_scrub_original.h. Fresh moving-scrub synthesized-speech PCM bit-identical at 0.025x, 0.08x, 0.5x, -0.5x, -1x; motion following a hold can differ briefly because the grain state has changed.
- Stationary overlap 6.25 ms vs 12.5 ms in R15. 14 ms phase-search radius retained to avoid low-frequency pitch failures. A 4 ms overlap and aggressive alignment locality candidates failed tone comparison. 5 ms passed the initial tone comparison but the 6.25 ms setting had lower mean speech source distance and better bass retention than the aggressive alternatives. No hard source window is imposed.
- Across 21 tone/pitch fixtures (65–880 Hz at half, neutral and double pitch), weighted mean source-center distance 4.957 to 3.193 ms; mean interpolation-center weight beyond 12.5 ms 0.0709 to 0.0046. All 21 met the comparison amplitude tolerance. Across 12 synthesized speech positions, mean source-center distance 7.686 to 6.142 ms; weight beyond 12.5 ms 0.1975 to 0.1022. These metrics describe weighted sample read centers, not exact audible leakage or sinc tap footprint. Some individual speech locations did not improve. Stereo synthetic polyphonic signal and motion/hold transitions tested; no real music listening test was performed.
- Source delta limited to r15_scrub.h and version identifiers in controller/plugin source. Effects and remaining DSP source unchanged. No claims of lossless holds or subjective AudioStretch parity. Native Windows and listening testing remains required.
- R14/R15 audio comparison files in validation are historical, not R16 listening results.

## Prior R15 validation (historical)

# R15 validation

The Windows x64 controller and VST DLL were cross-compiled using Clang/LLD, build 100214, PE file version 1.0.2.14. The final 37 C regression programs passed. Static source/manifest/version/host launch checks passed; the DLL exports VSTPluginMain. The historical comparison tests retain the older algorithms; assertions about the active engine and monitor bus were updated for the new semantics. See validation/test-results.txt, r15-pad-isolation.txt and r15-scrub-test.txt.

Pad isolation: the comparison of an unchanged playing pad and the same pad while selecting and editing another pad found a maximum summed microphone and monitor sample difference of 0.000000000, including effect tails. Linked mode uses per-pad gain, and unlink restores the saved independent master. The existing effect processing already uses the playing pad; effects_dsp.h is unchanged byte-for-byte from R14. New tests also cover simultaneous linked pads and selected-pad edits. This establishes mixer behavior in the offline harness, not the user's particular Windows configuration.

Scrub: neutral 220 Hz tone amplitude was 0.199948 or higher for all tested rates (input amplitude 0.2), including zero, 0.025x, 0.08x, forward and reverse. The shared stereo ratio error was 0 in the proportional-channel fixture; the independent octave settings retained their target tone. The exact 1x PCM test showed zero error in the core renderer. The silent 75 ms hold region, quantized mouse positions, 44.1-to-48 kHz source/host mismatch and synthesized speech tests passed. These are controlled checks, not a perceptual fidelity rating. The engine retains the existing application-level scratch headroom. No claim of lossless extreme stretching or AudioStretch equivalence is made.

The optional WAV files in validation/ compare the bundled eSpeak speech fixture at a neutral pitch, source 1x (1.5 seconds), and R14/R15 at 0.25x (6 seconds). Both renders use the core processors, before the application-level headroom, effects and limiter, with no loudness normalization. Their generator is tests/render_r15_comparison.c. The R14 historical logs are retained as references. The source fixture and WAV files are synthesized speech, not real human recordings.

Bungee could not be retrieved in the build environment and was not integrated. No dedicated formant correction is implemented; non-neutral pitch changes can change formants. Repeated texture, artifacts on transients or stationary holds, and adjacent audio can still occur. No Windows runtime test, live listening comparison, actual host latency/CPU or device driver test was performed. AudioStretch comparison remains outstanding. Existing R14 UI limitations (very large fonts may clip, whole-theme/dropdown opening animations unimplemented) remain. The older font trailer's 11 pt pad setting is migrated to 10 pt; other values are preserved. The new font trailer tag requires a pre-upgrade configuration backup for rollback to R14. The stock Equalizer APO host panel behavior is unchanged.

Additional exploratory analysis on a local speech sample from the earlier audit (provenance not verified, not redistributed): a normalized autocorrelation pitch estimate was compared to the corresponding source window in 269 strongly voiced reference frames at 0.25x speed. R14 and R15 each had 234 frames within 50 cents; R14 had 8 paired frames more than 300 cents off and R15 had 5. Pitch estimates were absent in 1 R14 and 3 R15 frames. This result is mixed and does not demonstrate superior speech quality.

## R18 results

41/41 offline C programs passed. Both Windows x64 binaries cross-compiled and passed static version, manifest, and export checks. The updated cursor test exercises actual scratch_update_position with duplicate ticks, irregular gaps to 220 ms, reversal, silence, and release. Shorter slow-motion overlap was selected after 24 moving bass/pitch cases passed. A narrower alignment search was rejected after a 110 Hz half-pitch test showed amplitude loss. At 0.025x and 0.08x, the weighted contribution beyond 12.5 ms fell from 29.14% to 20.64% and from 27.12% to 17.34% respectively on the synthesized speech fixture. Faster tested motion and stationary overlap are unchanged; actual audible leakage is not established by this measurement. Effect DSP header unchanged. Historical R16 equivalence test now uses the frozen R17 reconstruction, and the R17 gate test is updated for R18 activity deadlines.

R19 final validation: 42/42 offline C test programs passed. Static version, export and launch checks passed.
