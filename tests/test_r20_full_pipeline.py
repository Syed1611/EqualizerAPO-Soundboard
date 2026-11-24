"""R21: compare neutral portion to R2, verify finite output after intentional pitch change."""
from pathlib import Path
import subprocess, tempfile, itertools, struct, math
root=Path(__file__).resolve().parent.parent
with tempfile.TemporaryDirectory() as tmp:
    paths=[]
    for original in (True,False):
        exe=Path(tmp)/("r2" if original else "r20")
        args=["clang","-O1","-fdeclspec","-I.","-Wno-incompatible-library-redeclaration","-Wno-ignored-attributes"]
        if original: args.append("-DORIGINAL_R2")
        subprocess.run(args+["tests/render_r20_reference.c","-lm","-o",str(exe)],cwd=root,check=True)
        paths.append(exe)
    for rate,pitch,takeover in itertools.product((48000,44100),(0,9,-12),(0,1)):
        samples=[subprocess.check_output([str(exe),str(rate),str(pitch),str(takeover)]) for exe in paths]
        assert all(len(s)==240000 for s in samples),(rate,pitch,takeover)
        if pitch==0:
            assert samples[0][:65*480*4]==samples[1][:65*480*4],(rate,pitch,takeover)
        assert all(math.isfinite(x) for x in struct.unpack("<60000f",samples[1]))
        print(f"PASS rate={rate}, half-semitones={pitch}, takeover={takeover}: {'neutral prefix identical; ' if pitch==0 else ''}finite 60000 float samples")
