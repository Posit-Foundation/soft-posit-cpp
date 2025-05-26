import softposit as sp
import numpy as np
import lut_parameters


FUNC_NAME = "p8_log2"
LUT_NAME = "p8log2"
HEADER_FILENAME = rf"../include/p8_LUTs/{{FUNC_NAME}}_LUT.h"

LUT = []

for i in range(128):  # Only valid region
    p8 = sp.posit8()
    p8.fromBits(i)

    val = float(p8)
    if val <= 0 or not np.isfinite(val):
        LUT.append(128)
        continue

    logval = np.log2(val)
    if logval < -16: logval = -16
    if logval > 16:  logval = 16
    if abs(logval) < 1e-8: logval = 0.0

    out = sp.posit8(logval)
    LUT.append(out.v.v)
    
# Print C-style
def format_lut_line(values):
    return "    " + "".join(f"{{v}},".ljust(6) for v in values) + "\n"


with open(HEADER_FILENAME, "w") as f:
    f.write(f"#ifndef P8_P8_TAN_LUT_H\n")
    f.write(f"#define P8_P8_TAN_LUT_H\n\n")
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