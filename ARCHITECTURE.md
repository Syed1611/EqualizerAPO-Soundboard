# Scratch Edition architecture

Equalizer APO loads the VST2 DLL in the capture chain. The DLL requests soundboard PCM from the controller over a local named pipe, validates protocol-v3 message layouts and mixes audio with the microphone. The controller owns sample loading, pad configuration, effects, global hotkeys, UI and local monitoring.

`src/controller.c` implements transport, mixer, monitor, Win32 UI and configuration. `src/plugin.c` implements the VST2 ABI, host launch and pipe stream. `src/winlite.h` contains the freestanding Win32 resolver. `src/effects_dsp.h` contains per-pad reverb/echo. `src/r19_restore.h` namespaces the active R2-derived pitch core in `src/r21_r2_pitch.h`; the original R2 reference is kept in `src/r19_r2_original.h`. Legacy research engines and generated tables remain for regression tests, not selectable active scrub modes.

The scrub path uses cached host-rate mono samples and original R2 timing/FFT/WSOLA. The non-neutral pitch interpolation is a 64-tap band-limited filter. The Silent gate modifies output gain only. Ordinary pad playback remains stereo. Linked gain is resolved per playing pad; unlinked monitor gain is global.

The configuration path remains Scratch-specific, with backup/CRC recovery. The private IPC pipe, class and mutex are retained to support known-compatible older DLLs; v1.1 internal build is 110000. The standard edition has separate source and configuration.
