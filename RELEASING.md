# Releasing Scratch Edition v1.1

- Stable source: `scratch` branch. Lightweight source: `main`.
- Build the Windows x64 controller and DLL using BUILDING.txt. Run the offline suite and Windows acceptance checklist.
- Package `APOSoundboard-ScratchEdition-v1.1-stable-x64.zip` with both matching binaries, sources, resource files, documentation, tests and SHA-256 manifest. Verify archive integrity and checksums.
- Suggested release tag: `scratch-v1.1`; title: `APOSoundboard Scratch Edition v1.1`. Use the actual stable source commit, not `main`, as the tag target.
- Publish the ZIP only after the maintainer tests the Windows build. Do not publish intermediate development builds.
- Release creation, tagging and asset upload are performed by the repository maintainer.
