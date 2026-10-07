#!/usr/bin/env python3
import sys
from pathlib import Path

if len(sys.argv) != 3:
    raise SystemExit("usage: patch_eapo_ui.py <repo-root> <a|b>")

root = Path(sys.argv[1])
variant = sys.argv[2].lower()
if variant not in {"a", "b"}:
    raise SystemExit("variant must be a or b")

gui = root / "Editor/guis/VSTPluginFilterGUI.cpp"
dlg = root / "Editor/guis/VSTPluginFilterGUIDialog.cpp"

text = gui.read_text(encoding="utf-8")
old = """\t\tVSTPluginFilterGUIDialog dialog(this, effect, autoApplyDialog);
\t\tconnect(dialog.getApplyButton(), SIGNAL(pressed()), SLOT(applyDialog()));
\t\tconnect(dialog.getAutoApplyCheckBox(), SIGNAL(toggled(bool)), SLOT(autoApplyToggled(bool)));
\t\tlastReadTimer.invalidate();
"""
new = """\t\tconst QString effectName = QString::fromStdWString(effect->getName());
\t\tconst bool appMicPanel = effectName.startsWith(\"AppMic\", Qt::CaseInsensitive);
\t\tif (appMicPanel)
\t\t\tautoApplyDialog = false;

\t\tVSTPluginFilterGUIDialog dialog(this, effect, autoApplyDialog);
\t\tif (!appMicPanel)
\t\t{
\t\t\tconnect(dialog.getApplyButton(), SIGNAL(pressed()), SLOT(applyDialog()));
\t\t\tconnect(dialog.getAutoApplyCheckBox(), SIGNAL(toggled(bool)), SLOT(autoApplyToggled(bool)));
\t\t}
\t\tlastReadTimer.invalidate();
"""
if old not in text:
    raise SystemExit("VSTPluginFilterGUI.cpp anchor not found")
gui.write_text(text.replace(old, new, 1), encoding="utf-8")

text = dlg.read_text(encoding="utf-8")
anchor = """\tQString name = QString::fromStdWString(effect->getName());
\tsetWindowTitle(name);
\tui->frame->setFixedSize(400, 300);
"""
if variant == "a":
    replacement = """\tQString name = QString::fromStdWString(effect->getName());
\tsetWindowTitle(name);
\tconst bool appMicPanel = name.startsWith(\"AppMic\", Qt::CaseInsensitive);
\tif (appMicPanel)
\t{
\t\tui->gridLayout->removeWidget(ui->buttonBox);
\t\tui->gridLayout->removeWidget(ui->autoApplyCheckBox);
\t\tdelete ui->buttonBox;
\t\tui->buttonBox = nullptr;
\t\tdelete ui->autoApplyCheckBox;
\t\tui->autoApplyCheckBox = nullptr;
\t\tui->gridLayout->setRowMinimumHeight(1, 0);
\t\tui->gridLayout->setRowStretch(1, 0);
\t}
\tui->frame->setFixedSize(400, 300);
\tif (appMicPanel)
\t{
\t\tui->gridLayout->activate();
\t\tadjustSize();
\t}
"""
else:
    replacement = """\tQString name = QString::fromStdWString(effect->getName());
\tsetWindowTitle(name);
\tconst bool appMicPanel = name.startsWith(\"AppMic\", Qt::CaseInsensitive);
\tif (appMicPanel)
\t{
\t\tui->buttonBox->hide();
\t\tui->autoApplyCheckBox->hide();
\t}
\tui->frame->setFixedSize(400, 300);
\tif (appMicPanel)
\t{
\t\tui->gridLayout->activate();
\t\tadjustSize();
\t}
"""
if anchor not in text:
    raise SystemExit("VSTPluginFilterGUIDialog.cpp constructor anchor not found")
text = text.replace(anchor, replacement, 1)

if variant == "a":
    old_apply = """QPushButton* VSTPluginFilterGUIDialog::getApplyButton()
{
\treturn ui->buttonBox->button(QDialogButtonBox::Apply);
}

QCheckBox* VSTPluginFilterGUIDialog::getAutoApplyCheckBox()
{
\treturn ui->autoApplyCheckBox;
}
"""
    new_apply = """QPushButton* VSTPluginFilterGUIDialog::getApplyButton()
{
\tif (ui->buttonBox == nullptr)
\t\treturn nullptr;
\treturn ui->buttonBox->button(QDialogButtonBox::Apply);
}

QCheckBox* VSTPluginFilterGUIDialog::getAutoApplyCheckBox()
{
\treturn ui->autoApplyCheckBox;
}
"""
    if old_apply not in text:
        raise SystemExit("VSTPluginFilterGUIDialog.cpp accessor anchor not found")
    text = text.replace(old_apply, new_apply, 1)

    old_slot = """void VSTPluginFilterGUIDialog::on_autoApplyCheckBox_clicked(bool checked)
{
\tif (checked)
\t\tgetApplyButton()->click();
}
"""
    new_slot = """void VSTPluginFilterGUIDialog::on_autoApplyCheckBox_clicked(bool checked)
{
\tif (checked)
\t{
\t\tQPushButton* applyButton = getApplyButton();
\t\tif (applyButton != nullptr)
\t\t\tapplyButton->click();
\t}
}
"""
    if old_slot not in text:
        raise SystemExit("VSTPluginFilterGUIDialog.cpp slot anchor not found")
    text = text.replace(old_slot, new_slot, 1)

old_start = """\tif (effect->startEditing(hwnd, &width, &height))
\t{
\t\tui->frame->setFixedSize(width, height);
\t\teffect->setSizeWindowFunc(bind(&VSTPluginFilterGUIDialog::onSizeWindow, this, _1, _2));
\t\tidleTimer.start();
\t\treturn;
\t}
"""
new_start = """\tif (effect->startEditing(hwnd, &width, &height))
\t{
\t\tui->frame->setFixedSize(width, height);
\t\tif (windowTitle().startsWith(\"AppMic\", Qt::CaseInsensitive))
\t\t\tadjustSize();
\t\teffect->setSizeWindowFunc(bind(&VSTPluginFilterGUIDialog::onSizeWindow, this, _1, _2));
\t\tidleTimer.start();
\t\treturn;
\t}
"""
if old_start not in text:
    raise SystemExit("VSTPluginFilterGUIDialog.cpp startEditor anchor not found")
text = text.replace(old_start, new_start, 1)

dlg.write_text(text, encoding="utf-8")
print(f"patched Equalizer APO editor variant {variant}")
