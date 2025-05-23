#!/usr/bin/env python
# Basic test for SoftPosit Python wrapper

import sys
import os

# Try to import the module
try:
    import _softposit
    import softposit

    print("Successfully imported softposit modules")
except ImportError as e:
    print(f"Import error: {e}")
    sys.exit(1)

# Try to create a posit8 value
try:
    p8 = softposit.posit8(1.5)
    print(f"Created posit8(1.5) = {p8}")
    sys.exit(0)
except Exception as e:
    print(f"Error creating posit8: {e}")
    sys.exit(1)
