# Publishing GitHub Releases

Source history is committed separately for each version. The download packages are maintained as **GitHub Release assets**, not files checked into the source tree.

## New versions to publish

| Tag | Release title | Target commit | Asset |
| --- | --- | --- | --- |
| `v0.8.8` | APO Soundboard v0.8.8 — Auto Sweep | `34787dd90dc0bddefe35f0cf8557dc991b553443` | `APOSoundboard-v0.8.8-x64.zip` |
| `v0.8.9` | APO Soundboard v0.8.9 — UI & Playback Fixes | `c174cee522ed3ba6ca4edcf6c374d6b0151d69cc` | `APOSoundboard-v0.8.9-x64.zip` |
| `v0.8.10` | APO Soundboard v0.8.10 — Control Polish | `242af02c218e4c8d648ebbeabb8c685feb25533d` | `APOSoundboard-v0.8.10-x64.zip` |
| `v0.9.0` | APO Soundboard v0.9.0 — Dark Mode | `81cca58811cd6e4cb58f10f91f7f3e857677417e` | `APOSoundboard-v0.9.0-x64.zip` |

Each tag should point to the **corresponding version commit**, not all to `main`. If the GitHub Release editor cannot select a specific commit for tag creation, create that tag separately before drafting the release.

1. Open the repository's **Releases → Draft a new release** page.
2. Choose/create the exact tag above, targeting the corresponding commit.
3. Use the corresponding title; set `v0.9.0` as **Latest**.
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
