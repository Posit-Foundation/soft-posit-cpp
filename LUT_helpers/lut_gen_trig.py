import softposit as sp
import math
import numpy as np


LUT = []
for i in range(0, 256):
    p8 = sp.posit8()
    p8.fromBits(i)

    # Check if the posit is NaR (Not a Real number) ----- alternate approach - need to check
    if p8.isNaR():
        LUT.append(128)  # 0x80 for NaR
        continue

    num = float(p8)
    # print("Float from posit:", num)

    if num == float('inf') or num == float('-inf') or num == float('nan'):
        # print(f"Exception occured when converting the number {i}")
        LUT.append(128)
        continue

    float_val = np.sin(np.pi * num) 
    if abs(float_val) < 1e-12:
        float_val = 0.0

    if abs(float_val - 1.0) < 1e-5:
        float_val = 1.0
    elif abs(float_val + 1.0) < 1e-5:
        float_val = -1.0
    elif abs(float_val) < 1e-7:
        float_val = 0.0

    # Convert exp result to posit8
    p8_LUT = sp.posit8(float_val)

    # Extract bits from posit8 object and format to binary string
    bits = p8_LUT.v.v  # raw 8-bit unsigned integer
    binary_str = f"{bits:08b}"
    integer_value = int(binary_str, 2)

    LUT.append(integer_value)

# === Format as C-style array ===
def format_val(val):
    if val < 10:
        return f"{val},   "
    elif val < 100:
        return f"{val},  "
    else:
        return f"{val}, "

for i in range(0, 256, 16):
    row = LUT[i:i+16]
    formatted_row = "    " + "".join(format_val(v) for v in row)
    print(formatted_row)