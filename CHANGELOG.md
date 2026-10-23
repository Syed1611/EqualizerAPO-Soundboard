# Scratch Edition v0.9.1 — Experimental

- Forked the controller/VST from the uploaded v0.9.0 release without changing it.
- Isolated VST unique ID, IPC named pipe, controller executable/instance,
  configuration file and window class from the stable edition.
- Added Select Range and Scratch & Freeze waveform modes.
- Scratch controls pitch, speed and direction via mouse movement.
- Holding the waveform motionless performs continuous two-grain audio freeze,
  rather than going silent; release crossfades back to ordinary playback.
- Scratching takes over an existing pad voice to avoid unmodified doubling.
- Bold, centered 16px Speed/Depth numeric fields.
- Existing waveform range, local monitor, live pitch, hotkeys and dark mode kept.

Note: experimental audio quality/latency may require tuning on Windows.
