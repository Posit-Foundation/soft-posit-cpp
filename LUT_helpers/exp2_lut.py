import softposit as sp
import numpy as np

LUT = []

for i in range(256):
    p8 = sp.posit8()
    p8.fromBits(i)

    if p8.isNaR():
        LUT.append(128)  # NaR
        continue

    num = float(p8)
    if not np.isfinite(num):
        LUT.append(128)
        continue

    val = np.exp2(num)  # Equivalent to 2^x

    if not np.isfinite(val):
        LUT.append(128)
        continue

    # Clamp very small values to zero
    if abs(val) < 1e-8:
        val = 0.0

    # Convert result to posit8
    p8_result = sp.posit8(val)
    LUT.append(p8_result.v.v)

# Format as C-style LUT
def format_val(val):
    return f"{val},".ljust(6)

print(f"// Lookup Table for exp2(x) = 2^x using posit<8>\n")
for i in range(0, 256, 16):
    print("    " + "".join(format_val(v) for v in LUT[i:i+16]))
