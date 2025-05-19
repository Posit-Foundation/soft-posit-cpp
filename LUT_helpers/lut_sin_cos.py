import softposit as sp
import numpy as np

# Mode: "sin" or "cos"
MODE = "cos"

# Trig function selector
def trig_function(x, mode):
    if mode == "sin":
        return np.sin(np.pi * x)
    elif mode == "cos":
        return np.cos(np.pi * x)
    else:
        raise ValueError("Unsupported mode")

LUT = []
for i in range(256):
    p8 = sp.posit8()
    p8.fromBits(i)

    if p8.isNaR():
        LUT.append(128)  # 0x80 for NaR
        continue

    num = float(p8)
    if not np.isfinite(num):
        LUT.append(128)
        continue

    val = trig_function(num, MODE)

    # Clean small near-zero and ±1 errors due to float math
    if abs(val) < 1e-8:
        val = 0.0
    elif abs(val - 1.0) < 1e-5:
        val = 1.0
    elif abs(val + 1.0) < 1e-5:
        val = -1.0

    # Convert result to posit8
    p8_result = sp.posit8(val)
    LUT.append(p8_result.v.v)

# Format output as C-style LUT
def format_val(val):
    return f"{val},".ljust(6)

print(f"// Lookup Table for {MODE}(π·x) with posit<8>\n")
for i in range(0, 256, 16):
    row = LUT[i:i+16]
    print("    " + "".join(format_val(v) for v in row))
