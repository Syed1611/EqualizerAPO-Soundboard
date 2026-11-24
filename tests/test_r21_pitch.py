"""Neutral equivalence and alias suppression against original R2, no external Python packages."""
from pathlib import Path
import subprocess,tempfile,struct,math
root=Path(__file__).resolve().parent.parent
with tempfile.TemporaryDirectory() as d:
    exes=[]
    for original in (True,False):
        exe=Path(d)/('before' if original else 'after')
        subprocess.run(['clang','-O2']+(['-DORIGINAL_R2'] if original else [])+['tests/render_r21_pitch.c','-lm','-o',str(exe)],cwd=root,check=True)
        exes.append(exe)
    def run(pitch,speed):return [subprocess.check_output([str(exe),str(pitch),str(speed),'96000']) for exe in exes]
    for speed in (0,.25,-.25):
        a,b=run(0,speed);assert a==b;print(f'PASS neutral pitch speed {speed}: 96000 samples identical')
    a,b=run(12,0)
    def db(raw,hz):
        x=struct.unpack('<96000f',raw)[-48000:];n=len(x)
        w=[.5-.5*math.cos(2*math.pi*i/(n-1)) for i in range(n)]
        y=[v*t for v,t in zip(x,w)];q=2*math.pi*hz/48000
        return 20*math.log10(max(1e-20,2*math.hypot(sum(v*math.cos(q*i) for i,v in enumerate(y)),sum(v*math.sin(q*i) for i,v in enumerate(y)))/sum(w)))
    wanted=[db(x,8000) for x in (a,b)];alias=[db(x,16000) for x in (a,b)]
    print(f'Wanted 8 kHz {wanted[0]:.3f} -> {wanted[1]:.3f} dBFS; alias 16 kHz {alias[0]:.3f} -> {alias[1]:.3f} dBFS; reduction {alias[0]-alias[1]:.3f} dB')
    assert abs(wanted[0]-wanted[1])<.05 and alias[0]-alias[1]>60
    for pitch in (-12,7,12):
        for speed in (.25,-.25):run(pitch,speed)
    print('PASS finite output at non-neutral pitch, forward/reverse')
