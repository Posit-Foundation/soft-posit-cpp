import softposit as sp
import numpy as np
import lut_parameters


FUNC_NAME = "p8_exp2"
LUT_NAME = "p8exp2"
HEADER_FILENAME = rf"../include/p8_LUTs/{FUNC_NAME}_LUT.h"
LUT = []

for i in range(256):
    p8 = sp.posit8()
    p8.fromBits(i)

    if p8.isNaR():
        LUT.append(128)  # NaR
        continue

    num = float(p8)
    
    if (
        p8.isNaR()
        or not np.isfinite(num)
        or num < -8.0
        or num > 6.0
    ):
        LUT.append(128)
        continue

    val = np.exp2(num)

    if not np.isfinite(val) or val > float(lut_parameters.p8.positive_maxpos):
        LUT.append(128)
        continue

    if abs(val) < lut_parameters.clamps.zero_clamp:
        val = 0.0

    # Convert result to posit8
    p8_result = sp.posit8(val)
    LUT.append(p8_result.v.v)

# Write the header file directly in place
def format_lut_line(values):
    return "    " + "".join(f"{v},".ljust(6) for v in values) + "\n"

with open(HEADER_FILENAME, "w") as f:
    f.write(f"#ifndef P8_{FUNC_NAME.upper()}_LUT_H\n")
    f.write(f"#define P8_{FUNC_NAME.upper()}_LUT_H\n\n")
    f.write("#include <stdint.h>\n\n")
    f.write("#ifdef __cplusplus\n")
    f.write('extern "C" {\n')
    f.write("#endif\n\n")
    f.write(f'static const uint8_t {LUT_NAME}[256] = {{\n')

    for i in range(0, 256, 16):
        f.write(format_lut_line(LUT[i:i+16]))

    f.write("};\n\n")
    f.write("#ifdef __cplusplus\n")
    f.write("}\n")
    f.write("#endif\n\n")
    f.write("#endif\n")