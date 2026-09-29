# Architecture

APO Soundboard intentionally uses two small Windows processes/components:

1. **VST DLL** - loaded by Equalizer APO in the microphone/capture processing chain. It passes the microphone through and mixes active soundboard voices into that stream.
2. **Controller EXE** - owns the pad UI, audio-file decoding, global hotkeys, waveform regions and persistent configuration.

The two communicate through a local Windows named pipe. Audio does not pass through a virtual cable/device; the final mix is performed by the VST in Equalizer APO's capture chain.

This split exists because Equalizer APO's Configuration Editor/editor instance is separate from the continuously running DSP instance. Keeping hotkeys and user-file management in the controller avoids relying on DAW-style host services that Equalizer APO does not provide.

## Resource model

- Bounded decoded sample RAM: 128 MiB per pad / 512 MiB total.
- Win32/GDI UI; no Direct3D/OpenGL/Vulkan rendering path.
- Media Foundation is used only for file decoding when needed and is explicitly shut down.
- Current VST worker shutdown waits for the worker to leave before freeing the owning instance.