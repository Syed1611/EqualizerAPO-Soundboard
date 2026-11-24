"""Static release assertions; not a substitute for a Windows GUI/UAC test."""
from pathlib import Path
import re
from zipfile import ZipFile

root = Path(__file__).resolve().parent.parent
plugin = (root / "src/plugin.c").read_text()
controller = (root / "src/controller.c").read_text()
patch = (root / "host-integration/EqualizerAPO-OpenPanel-direct.patch").read_text()
assert "#define BUILD_VERSION 110000u" in plugin and "#define BUILD_VERSION 110000u" in controller
for f in (plugin, controller):
    assert "'h','_','v','1','2','s',0" in f
    assert "'h','C','t','r','l','R','2','0',0" in f
assert "'r','_','v','1','0','2','s',0" in controller
assert 'apo_init_editor_api(&p->editorApi)' in plugin and 'wl_init_gui(&p->editorApi)' not in plugin
assert 'APO_CreateProcessW create=' in plugin
assert 'create(path,0,0,0,WL_FALSE,0,0,folder,&si,&pi)' in plugin
assert 'ShellExecuteW(0' not in plugin and 'editorChild' not in plugin
assert 'GetWindowTextW(root,title,64)' in plugin
assert 'APO_GW_OWNER' in plugin and 'APO_WS_POPUP' in plugin
assert 'PostMessageW(root,APO_WM_CLOSE' in plugin
assert 'effect->uniqueID() == scratchId' in patch
assert 'effect->getName() == L"APO Soundboard Scratch"' in patch
assert 'effect->startEditing(NULL,' in patch
assert 'effect->stopEditing()' in patch
assert 'if(ptr) dismiss_standalone_host_panel(p,(WL_HWND)ptr);' in plugin
assert 'if(!ptr)return 0;' not in plugin[plugin.index("case effEditOpen:{"):plugin.index("case effEditClose:")]
assert 'VSTPluginFilterGUIDialog dialog(this, effect, autoApplyDialog)' in patch
manifest = (root / "Controller.asInvoker.manifest").read_text()
exe = (root / "APOSoundboardScratchController.exe").read_bytes()
dll = (root / "APOSoundboardScratch_v1.1.dll").read_bytes()
assert 'level="asInvoker"' in manifest and 'uiAccess="false"' in manifest
assert b'level="asInvoker"' in exe and b'uiAccess="false"' in exe
assert exe[:2] == b'MZ' and dll[:2] == b'MZ'
assert "1.1.0.0".encode("utf-16le") in exe and "1.1.0.0".encode("utf-16le") in dll
assert len(list((root / "tests").glob("test_*.c"))) == 44
assert "APO Soundboard Scratch Edition v1.1".encode("utf-16le") in exe
assert "Use Original R2 scrub".encode("utf-16le") not in exe
assert "Ultra Quality scrub (experimental)".encode("utf-16le") not in exe
assert "Reset high-volume approval".encode("utf-16le") in exe
assert b"Microsoft.Windows.Common-Controls" in exe
assert "Localized R2 hold (experimental)".encode("utf-16le") not in exe
assert "Segoe UI Variable".encode("utf-16le") in exe or b"Segoe UI Variable" in exe
assert "APO Soundboard Scratch v1.1" in controller
assert "R2 Max hold (experimental)".encode("utf-16le") not in exe
print("PASS: v1.1 source/build identifiers, guarded host launch, 44 C test sources, host patch and asInvoker resource")

assert "static int r10_quality(void){return 3;}" in controller
assert "monitor_process(c,c->monitorAudio,rq.frames" in controller

assert "ID_R17_HOLD 1122u" in controller
assert "g_r17HoldButton" in controller and "r17_gate" in controller

assert (root / "src/r19_r2_original.h").read_bytes() == (root / "tests/reference/r2_scratch_dsp.h").read_bytes()
assert "r19_scratch_stretch_render(&v->scrub" in controller
assert "g_volW=availW-labelW-linkW-ui_scale(10)" in controller
