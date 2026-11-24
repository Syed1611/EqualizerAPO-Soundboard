# Optional Equalizer APO Configuration Editor source patch

**Purpose:** Remove the brief VST editor-dialog flash when **Open Panel** is clicked for **APO Soundboard Scratch**. This is a **source patch**, not an Equalizer APO executable and not an automatic installer. It is not needed for the soundboard's audio processing or standalone controller.

## Why a host patch is needed

The official Equalizer APO source (`Editor/guis/VSTPluginFilterGUI.cpp`, method `on_openPanelButton_clicked`) constructs `VSTPluginFilterGUIDialog` and calls `dialog.exec()`. That happens on the Equalizer APO side. The VST receives `effEditOpen` **after** the dialog has been constructed and cannot prevent it from appearing. R3–R19's plugin-side request to close the owned dialog can shorten its lifetime but cannot guarantee zero flicker.

The patch modifies only `on_openPanelButton_clicked` for the VST whose unique ID is `APSX` and whose name is `APO Soundboard Scratch`. It calls `VSTPluginInstance::startEditing` with a null HWND (R8 explicitly supports a parentless launch) and immediately calls `stopEditing`. The Scratch VST interprets `effEditOpen` as “launch or focus its standalone controller.” **No QDialog is constructed for this plugin**. All other VST plugins retain the original dialog.

## How to use it (advanced, optional)

1. Obtain the **matching Equalizer APO source** for your installed version. Back up the installed Configuration Editor before replacing any executable. The patch targets the public source's `Editor/guis/VSTPluginFilterGUI.cpp` as inspected on 2026-11-25; its layout may differ on other branches or releases.
2. At the source repository root, validate with `git apply --check path/to/EqualizerAPO-OpenPanel-direct.patch`. Apply using `git apply path/to/EqualizerAPO-OpenPanel-direct.patch` if it passes.
3. Rebuild the **Configuration Editor** with the Qt/MSVC setup required by that Equalizer APO version. The stock audio APO service, drivers, VST binary and registration need no replacement. Do **not** replace the Configuration Editor with a partially linked or incompatible build.
4. Run the rebuilt Editor on Windows, load the **R19 VST DLL** and press Open Panel. The controller should launch or focus directly with no Qt dialog. Verify that Open Panel on **other VST plugins** still opens their original editors.
5. Restore the backed-up original Configuration Editor if the customized build does not behave correctly.

The corrected source patch passed a dry run against an excerpt reproducing the published handler context. The public VSTPluginInstance::startEditing implementation forwards its HWND to effEditOpen. R8 through R19 accept a null HWND; earlier revisions do not. Revert this patch before using older Scratch Edition DLLs. An Equalizer APO Qt/MSVC build or real Windows host test **has not been run** here. The exact matching source revision must be checked before use.

**Security:** The patched Editor does not bypass UAC, Windows permissions or anti-malware checks. The R8 controller's manifest requests `asInvoker`; Windows policy can still require administrator approval for unrelated operations.

Original public source: https://github.com/mirror/equalizerapo/blob/master/Editor/guis/VSTPluginFilterGUI.cpp

Equalizer APO code is GPL-2.0-or-later; apply and distribute any modified Editor in accordance with that license. The patch file itself is provided under the same license for compatibility.

**Alternative without modifying Equalizer APO:** Start or pin `APOSoundboardScratchController.exe` directly. That launches the controller without any VST editor dialog, but it does not change Equalizer APO's Open Panel button.
