import softposit as sp
import numpy as np
import lut_parameters


FUNC_NAME = "p8_sin"
LUT_NAME = "p8sin"
HEADER_FILENAME = rf"../include/p8_LUTs/{{FUNC_NAME}}_LUT.h"
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

    val = np.sin(num)

    if not np.isfinite(val):
        LUT.append(128)
        continue

    if abs(val - 1.0) < lut_parameters.clamps.zero_clamp:
        val = 1.0
    elif abs(val + 1.0) < lut_parameters.clamps.zero_clamp:
        val = -1.0
    elif abs(val) < lut_parameters.clamps.zero_clamp:
        val = 0.0

    p8_result = sp.posit8(val)
    LUT.append(p8_result.v.v)

def format_lut_line(values):
    return "    " + "".join(f"{{v}},".ljust(6) for v in values) + "\n"

with open(HEADER_FILENAME, "w") as f:
    f.write(f"#ifndef P8_P8_SIN_LUT_H\n")
    f.write(f"#define P8_P8_SIN_LUT_H\n\n")
    f.write("#include <stdint.h>\n\n")
    f.write("#ifdef __cplusplus\n")
    f.write('extern "C" {{\n')
    f.write("#endif\n\n")
    f.write(f"static const uint8_t {{LUT_NAME}}[256] = {{\n")

    for i in range(0, 256, 16):
        f.write(format_lut_line(LUT[i:i+16]))

    f.write("};\n\n")
    f.write("#ifdef __cplusplus\n")
    f.write("}\n")
    f.write("#endif\n\n")
    f.write("#endif\n")