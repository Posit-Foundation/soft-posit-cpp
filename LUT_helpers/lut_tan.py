import softposit as sp
import numpy as np

THRESHOLD = 1e6  # Adjusted threshold to utilize posit<8> range

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

    val = np.tan(np.pi * num)

    # Handle poles and large values
    if not np.isfinite(val) or abs(val) > THRESHOLD:
        LUT.append(128)
        continue

    # Clean small near-zero errors due to float math
    if abs(val) < 1e-8:
        val = 0.0

    # Convert result to posit8
    p8_result = sp.posit8(val)
    LUT.append(p8_result.v.v)

# Format output as C-style LUT
def format_val(val):
    return f"{val},".ljust(6)

print(f"// Lookup Table for tan(π·x) with posit<8>\n")
for i in range(0, 256, 16):
    row = LUT[i:i+16]
    print("    " + "".join(format_val(v) for v in row))
