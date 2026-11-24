# Publishing GitHub Releases

Source history is committed separately for each version. The download packages are maintained as **GitHub Release assets**, not files checked into the source tree.

## Published lightweight releases

| Tag | Release title | Target commit | Asset |
| --- | --- | --- | --- |
| `v0.8.8` | APO Soundboard v0.8.8 — Auto Sweep | `34787dd90dc0bddefe35f0cf8557dc991b553443` | `APOSoundboard-v0.8.8-x64.zip` |
| `v0.8.9` | APO Soundboard v0.8.9 — UI & Playback Fixes | `c174cee522ed3ba6ca4edcf6c374d6b0151d69cc` | `APOSoundboard-v0.8.9-x64.zip` |
| `v0.8.10` | APO Soundboard v0.8.10 — Control Polish | `242af02c218e4c8d648ebbeabb8c685feb25533d` | `APOSoundboard-v0.8.10-x64.zip` |
| `v0.9` | APO Soundboard v0.9.0 — Dark Mode | `0b68e1b7c62ab348e144a134806bacd053e5c0d3` | `APOSoundboard-v0.9.0-x64.zip` |

### Verified historical release tags

**Audited October 23, 2026:** The lightweight `v0.8.8`, `v0.8.9`, `v0.8.10`, and `v0.9` tags all point to their correct version-specific commits in the table above. GitHub's generated source archives now correspond to the historical version. No tag repairs are pending.

The Scratch Edition's eight source tags (`scratch-v0.9.1`–`scratch-v0.9.8`) were also verified against their historical version commits. See [Scratch Edition release index](https://github.com/Syed1611/EqualizerAPO-Soundboard/blob/experimental/scratch-freeze/SCRATCH_RELEASES.md).

Each tag must point to the **corresponding version commit**, not an unrelated branch head. The historical tags in this repository have already been verified.

1. Open the repository's **Releases → Draft a new release** page.
2. Choose/create the exact tag above, targeting the corresponding commit.
3. Use the corresponding title; keep the lightweight release tag `v0.9` as the repository-wide **Latest** if that is your preferred default.
4. Use the text in [release-notes/](release-notes/) and [CHANGELOG.md](CHANGELOG.md) for the release description.
5. Attach the matching **x64 ZIP** from your archived build (not GitHub's auto-generated Source code ZIP). Upload the matched DLL + controller together in the package.
6. Publish after confirming the ZIP runs on Windows/Equalizer APO.

The older experimental or superseded `v0.3.0`, `v0.4.0`, and `v0.8.3`–`v0.8.6` may be marked **Pre-release**. The four entries in the table above can use the normal label.

## Release ZIP SHA-256 checksums

```text
e3bfb6011945653499a284663c63be638f7c8052e315a914104226263c14b34c  APOSoundboard-v0.8.8-x64.zip
abd4122ac288f632245a8529befdc93709c725dd66eca0e9a873eb2c3d9f28b2  APOSoundboard-v0.8.9-x64.zip
0cc39db7697e93f05508b157716b6a4e0c6ab3e783887ada85c8a98d2c3809b1  APOSoundboard-v0.8.10-x64.zip
5c6d84b7347362e787981c5329e113549b190897e0180ded6d6f5e874f12ce96  APOSoundboard-v0.9.0-x64.zip
```

## Upgrade notes

Close the old controller and applications actively using the microphone. Extract the **entire package folder** under Equalizer APO's VSTPlugins folder, keeping the versioned VST DLL beside `APOSoundboardController.exe`. Change the VST selection in the Equalizer APO Configuration Editor to the new DLL, then choose **Open panel**. Preserve the previous version until the new one is tested.

OGG/Opus support still depends on Windows Media Foundation codecs. Native Ogg Vorbis and Opus decoders have **not** been bundled. The v0.9.0 appearance change does not alter the soundboard processing engine.

## Scratch Edition channel

The stable Scratch v1.1 source is on [`scratch`](https://github.com/Syed1611/EqualizerAPO-Soundboard/tree/scratch), separate from lightweight `main`. See the [Scratch release guide](https://github.com/Syed1611/EqualizerAPO-Soundboard/blob/scratch/RELEASING.md) and [release history](https://github.com/Syed1611/EqualizerAPO-Soundboard/blob/scratch/SCRATCH_RELEASES.md).

For the forthcoming release, choose `scratch-v1.1` as the tag, target the matching v1.1 source commit on `scratch`, and attach `APOSoundboard-ScratchEdition-v1.1-stable-x64.zip`. Do not tag `main` for Scratch. Publish only after local Windows/Equalizer APO acceptance. Previous published builds and tags remain unchanged; no intermediate v1.0.2 development revisions are being published.

GitHub's Latest label is repository-wide; use edition-specific release links where available. The lightweight v0.9.0 download remains [here](https://github.com/Syed1611/EqualizerAPO-Soundboard/releases/tag/v0.9).
