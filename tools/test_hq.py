"""Run the inherited DSP regression programs with HQ forced in a temporary header."""
import pathlib, shutil, subprocess, tempfile
root = pathlib.Path(__file__).resolve().parent.parent
names = "test_fidelity test_freeze_regression test_live_pitch test_motion_regression test_noise_detail test_sample_rates test_scratch test_scrub_hybrid test_scrub_reversal test_spectral_freeze test_v102_long_scrub test_v102r4_scrub".split()
with tempfile.TemporaryDirectory() as tmp:
    p = pathlib.Path(tmp)
    (p / "src").mkdir()
    for name in ("scratch_dsp.h", "effects_dsp.h", "r7_dsp_tables.h"):
        shutil.copyfile(root / "src" / name, p / "src" / name)
    h = p / "src" / "scratch_dsp.h"
    h.write_text(h.read_text().replace("sp->quality=0;", "sp->quality=1;"))
    for name in names:
        exe = p / name
        subprocess.run(["clang", "-O2", "-fdeclspec", "-I" + str(p), str(root / "tests" / (name + ".c")), "-lm", "-o", str(exe)], check=True)
        subprocess.run([str(exe)], cwd=p, check=True)
