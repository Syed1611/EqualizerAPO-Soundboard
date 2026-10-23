# Scratch Edition release maintenance

All eight Scratch releases (`scratch-v0.9.1`–`scratch-v0.9.8`) are published, with the matching x64 ZIP attached. See [SCRATCH_RELEASES.md](SCRATCH_RELEASES.md) for release links, version changes, source commit SHA values, and verified SHA-256 digests.

## Tag correction required

**Audited 2026-10-23:** All eight published Scratch tags currently resolve to lightweight `main` commit `acfc7e267abec010c3058a2c3aa8e4a27ca56ea4`, although they should resolve to their Scratch source commits. As a result GitHub's generated source archives are misleading. Attached x64 ZIPs have been checked and are correct.

The historical source commits were restored in the [Scratch release-history branch](https://github.com/Syed1611/EqualizerAPO-Soundboard/tree/experimental/scratch-release-history). The final v0.9.8 source is at `199bc3966deb59324034c8f26a52fc247b276a77` on `experimental/scratch-freeze`.

To fix tags from a clone with Git credentials and tag-write permissions, run:

```bash
git tag -f scratch-v0.9.1 193a7e8c9f6423a2bca6430dcf2a8861c428f434
git push --force origin refs/tags/scratch-v0.9.1
git tag -f scratch-v0.9.2 9cf9d6de51b450fec359bf212e20f36fd1ce8754
git push --force origin refs/tags/scratch-v0.9.2
git tag -f scratch-v0.9.3 81a4039da680bb58c74f6a27a80251564f56cf87
git push --force origin refs/tags/scratch-v0.9.3
git tag -f scratch-v0.9.4 15a186eb850d2e2a00b9e7f2b4a0d9f9d7f448c3
git push --force origin refs/tags/scratch-v0.9.4
git tag -f scratch-v0.9.5 0c70c7cd7cc13d721b471059e89a24798d8b810d
git push --force origin refs/tags/scratch-v0.9.5
git tag -f scratch-v0.9.6 400b77e4153cde5e174baa276c26a9ace7588d55
git push --force origin refs/tags/scratch-v0.9.6
git tag -f scratch-v0.9.7 dcb92136e72868b7f229879e17d2c4459c7bf033
git push --force origin refs/tags/scratch-v0.9.7
git tag -f scratch-v0.9.8 199bc3966deb59324034c8f26a52fc247b276a77
git push --force origin refs/tags/scratch-v0.9.8
```

**Warning:** These commands intentionally move existing published tag references. Verify you are in the correct repository first. Protected tags or immutable-release rules can block the update; if so, an administrator must handle it. The GitHub Release editor's *Target* dropdown does **not** automatically move a previously created tag. Do not delete/re-upload the verified ZIP assets.

After fixing tags, check that `git ls-remote --tags origin scratch-v0.9.8` points at `199bc396...` and that each other tag matches the corresponding commit in the index.

## Release channels and Latest

- **Lightweight v0.9.0:** [`v0.9`](https://github.com/Syed1611/EqualizerAPO-Soundboard/releases/tag/v0.9), source [`main`](https://github.com/Syed1611/EqualizerAPO-Soundboard/tree/main).
- **Scratch v0.9.8:** [`scratch-v0.9.8`](https://github.com/Syed1611/EqualizerAPO-Soundboard/releases/tag/scratch-v0.9.8), source [`experimental/scratch-freeze`](https://github.com/Syed1611/EqualizerAPO-Soundboard/tree/experimental/scratch-freeze).
- **Scratch historical versions:** v0.9.1–v0.9.7 are marked pre-release. v0.9.8 is a normal release.

GitHub uses **one repository-wide Latest release**; it cannot show a separate Latest for each branch. At the time of audit, `scratch-v0.9.8` is reported by GitHub as Latest. If you want lightweight v0.9.0 to be the default, open the [`v0.9` release](https://github.com/Syed1611/EqualizerAPO-Soundboard/releases/tag/v0.9), choose **Edit**, select **Set as latest release**, then **Update release**. Branch READMEs link their releases directly regardless of this label.

## Packaging and installation

Use the uploaded `APOSoundboard-ScratchEdition-v0.9.8-x64.zip` release attachment, not GitHub's auto-generated source archive, to install. Extract the complete folder under Equalizer APO's VSTPlugins directory. Keep `APOSoundboardScratch_v0.9.8.dll` alongside `APOSoundboardScratchController.exe`. The lightweight DLL/controller and Scratch DLL/controller should not be interchanged. The new edition uses different configuration and IPC components.

See [README.md](README.md), [BUILDING.txt](BUILDING.txt), [TESTING.txt](TESTING.txt), [AUDIT.txt](AUDIT.txt) and [CHANGELOG.md](CHANGELOG.md).
