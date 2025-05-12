import softposit as sp
import math
import numpy as np


LUT = []
for i in range(0, 256):
    p8 = sp.posit8()
    p8.fromBits(i)

    num = float(p8)
    # print("Float from posit:", num)
    if num == float('inf') or num == float('-inf') or num == float('nan'):
        LUT.append(128)
        continue

    # exp_val = math.exp(num)
    exp_val = np.sin(np.pi * num) 
    # print("exp(num):", exp_val)

    # Convert exp result to posit8
    p8_LUT = sp.posit8(exp_val)

    # Extract bits from posit8 object and format to binary string
    bits = p8_LUT.v.v  # raw 8-bit unsigned integer
    binary_str = f"{bits:08b}"
    integer_value = int(binary_str, 2)

    LUT.append(integer_value)

print("LUT:", LUT)
print("LUT length:", len(LUT))