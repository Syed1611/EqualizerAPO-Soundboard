# Scratch Edition — Published releases and maintenance

## Current release

**[Scratch Edition v1.0.0 — Final Release](https://github.com/Syed1611/EqualizerAPO-Soundboard/releases/tag/scratch-v1.0.0)** is published as a **regular release** and is the repository-wide Latest. The release tag `scratch-v1.0.0` points to the tested source snapshot `2ae349f374f4dd2e9fbfb5e7a3a93c1c5b9550f1`.

- Installer asset: [APOSoundboard-ScratchEdition-v1.0.0-x64.zip](https://github.com/Syed1611/EqualizerAPO-Soundboard/releases/download/scratch-v1.0.0/APOSoundboard-ScratchEdition-v1.0.0-x64.zip)
- Verified published asset SHA-256: `39b1f7d183871d0e0ae5deb85961d92175891414feafb6ea458d49f376745c67`
- Windows acceptance reported by the maintainer on 2026-10-23. Offline tests passed; this is not a blanket guarantee of support for every host/driver.
- The full-spectrum scratch DSP remains unchanged from the v0.9.10 build.

## Previous stable edition

**[Scratch v0.9.8 — UI Polish & Final Release](https://github.com/Syed1611/EqualizerAPO-Soundboard/releases/tag/scratch-v0.9.8)** was a working, stable Scratch Edition before the v1.0.0 controls. Keep its existing regular-release status and original assets/tags. Here “Final Release” is its historical title from before the later v1.0.0 milestone; v1.0.0 is the **current** final version.

The separate **[Lightweight v0.9.0](https://github.com/Syed1611/EqualizerAPO-Soundboard/releases/tag/v0.9)** remains a regular release on the independent `main` branch, optimized for lower-end systems.

## Intermediate, unpublished builds

- **v0.9.9:** Implemented Scrub click-to-takeover, release-to-pause, Play/Pause/Stop, selected-pad Space, and persistent cursor. **Known regression:** a mismatched fade and feedback during repeated re-entry can cause buzzing or an excessive audio spike. **Do not publish as a recommended installer**, even when documenting it as experimental.
- **v0.9.10:** Fixed repeated Scrub buzz, source PCM blending and fade weights while preserving all v0.9.9 controls. This is a usable historical pre-v1.0.0 hotfix but is already incorporated in the final release. **Publishing is optional**, not necessary for users.

If archiving **v0.9.10** for historical completeness, use:
- **Tag:** `scratch-v0.9.10` (must point to a separate, matching v0.9.10 **source** commit, not to v1.0.0 or v0.9.8).
- **Title:** `Scratch Edition v0.9.10 — Scrub Audio Hotfix (Experimental)`
- **Asset:** `APOSoundboard-ScratchEdition-v0.9.10-experimental-x64.zip` (verify it contains the matched v0.9.10 VST DLL and controller and its embedded source before uploading).
- **Prerelease:** Yes; **Latest:** No.
- **Summary:** “Fixes a harsh buzzing regression during repeated waveform dragging in v0.9.9 by correcting the entry crossfade and removing unintended audio feedback. Retains per-pad Scrub Play/Pause/Stop, Spacebar control, and pause-on-release. The full-spectrum high-fidelity DSP is unchanged. This release is archived for development history; v1.0.0 is recommended for everyday use.”

Do not generate a historical release tag using the current `experimental/scratch-freeze` branch without creating/identifying the matching earlier source commit. Otherwise GitHub’s automatically generated Source code ZIP will not match the uploaded old binaries. The original v0.9.10 archive includes its own source for reconstruction when needed.

## Other historical releases

The already published `scratch-v0.9.1`–`scratch-v0.9.7` releases remain historical prereleases, each with matching source tags and ZIP assets. Do not rewrite old tags or binaries.

GitHub offers only **one** repository-wide Latest designation. Current Latest is Scratch v1.0.0; the old lightweight and stable Scratch editions remain independently downloadable by their direct release links.

See [SCRATCH_RELEASES.md](SCRATCH_RELEASES.md) for the full verified history and [README.md](README.md) for current installation instructions.
