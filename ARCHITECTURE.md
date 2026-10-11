# Architecture

APO Soundboard intentionally uses two Windows components:

1. **VST DLL** - loaded by Equalizer APO in the microphone/capture processing chain. It passes the microphone through and mixes soundboard audio into that stream.
2. **Controller EXE** - owns the pad UI, file decoding, hotkeys, waveform regions, local monitoring, configuration and diagnostics.

The two communicate over a **local named pipe**. No virtual microphone/cable is required for the core microphone-routing path.

This split is intentional because Equalizer APO's editor/UI instance is separate from its continuously running DSP instance. Keeping user interaction and file management in the controller avoids relying on DAW-style host services that Equalizer APO does not provide.

## Controller / VST protocol

Current releases use **protocol v3**.

The VST supplies host telemetry such as:
- sample rate
- block size
- ring-buffer state
- adaptive target fill
- underruns
- peak/clipping data

The controller supplies generated stereo soundboard audio back to the VST. If the controller disconnects, the VST falls back to microphone-only passthrough and waits for reconnection.

## Audio engine

- Up to 32 active voices.
- Oldest-voice stealing when the pool is full.
- HQ cubic resampling.
- Live per-pad volume/pitch with short smoothing.
- Host-rate sample conversion where useful.
- Adaptive VST buffering.
- Real-time generation path avoids unnecessary heap allocation.
- Idle/deep-idle paths reduce work when no pads are playing.

## Auto Sweep and controller UI

The v0.8.8+ controller has a per-pad triangle-wave Auto Sweep (LFO) that modulates the existing live pitch parameter. The playback engine still uses sampler-style resampling: modulating pitch also changes playback rate/duration. **There is no independent time-stretch/speed engine.**

Auto Sweep has a cycle speed in Hz and depth in semitones. The pitch slider reflects its current position. Pad hotkeys also select the triggered pad in the controller without calling foreground activation.

The UI uses Win32/GDI, including owner-drawn buttons and waveform painting; Soundboard and Diagnostics views are responsive to window size.

v0.9.0 adds a Light/Dark palette, with native title-bar tinting where Windows supports it. The theme is persisted independently of the soundboard settings.

## Local monitoring

Local monitor playback is separate from the microphone/VST mix. Monitor failure must not block or interrupt microphone audio.

v0.9.0 preserves the compatibility-oriented Windows playback path from the v0.8.1/v0.8.2 line, with additional fallback handling retained. The controller attempts to avoid retaining an idle monitor source connection.

## File decoding

- WAV uses the built-in decoder.
- MP3/AAC/M4A/WMA/FLAC use Windows Media Foundation.
- OGG/OGA/Opus are accepted through Media Foundation when Windows exposes a compatible decoder.

There is currently no bundled native Vorbis/Opus decoder.

## Resource model

- 128 MiB maximum decoded sample RAM per pad.
- 512 MiB maximum decoded sample RAM in total.
- Background decode/resample work is bounded.
- Sample replacement uses deferred reclamation so data is not freed while the mixer may still reference it.
- Win32/GDI UI; no Direct3D/OpenGL/Vulkan renderer.

## Configuration

The compatibility filename remains:

`%LOCALAPPDATA%\APOSoundboard\config_v05.bin`

The internal format is versioned separately (v11 in v0.9.0). Current builds use atomic replacement, CRC validation and `.bak` recovery so interrupted/corrupt writes can fall back to the previous known-good configuration.

The Light/Dark preference is stored separately as `%LOCALAPPDATA%\APOSoundboard\config_v05.bin.theme` for compatibility with older configs.
