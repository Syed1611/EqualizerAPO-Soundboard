APO Soundboard v0.6.1 x64 - Stability Audit Build
==================================================

This is v0.6 behavior/UI with stability hardening only. No installer.
Keep APOSoundboard_v0.6.1.dll and APOSoundboardController.exe together.

Stability changes:
- VST pipe thread must stop before the VST instance can be freed (prevents use-after-free).
- Media Foundation and COM are shut down cleanly when the controller exits.
- The single-instance mutex is closed cleanly.
- Worker threads no longer call GUI SetWindowText directly; connection UI updates are posted to the UI thread.
- Decoded sample RAM is bounded to 128 MiB per pad and 512 MiB total to prevent runaway memory pressure from long files.
- Idle pipe polling is reduced slightly to lower wakeups/CPU use.
- Same config_v05.bin format and same v0.6 controls/behavior.

Security/static audit notes:
- No networking APIs.
- No registry writes or startup persistence.
- No process injection, remote-thread, credential, or downloader code.
- UI uses Win32/GDI only; no Direct3D/OpenGL/Vulkan GPU path.
- The binaries use a tiny dynamic Win32 API resolver, so their PE import table is intentionally empty.
  This is unusual but comes from the included source/build method, not from packed/encrypted payloads.

This build was cross-compiled and statically checked, but cannot be runtime-tested
inside Equalizer APO in the build environment.