import softposit as sp
import numpy as np

log2_lut = []

for i in range(128):  # Only valid region
    p8 = sp.posit8()
    p8.fromBits(i)

    val = float(p8)
    if val <= 0 or not np.isfinite(val):
        log2_lut.append(128)
        continue

    logval = np.log2(val)
    if logval < -16: logval = -16
    if logval > 16:  logval = 16
    if abs(logval) < 1e-8: logval = 0.0

    out = sp.posit8(logval)
    log2_lut.append(out.v.v)
    
# Print C-style
for i in range(0, 128, 16):
    print("    " + ", ".join(f"{v:3d}" for v in log2_lut[i:i+16]) + ",")
