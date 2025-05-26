from dataclasses import dataclass

@dataclass
class clamps:
    zero_clamp: float = float(1/64)
    inf_clamp: float = float(64)

class p8:
    positive_maxpos: float = float(64)
    positive_minpos: float = float(1/64)