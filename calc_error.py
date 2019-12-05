import sys
from decimal import *

filename = "fpbench_results/" + sys.argv[1]
doubles = []
floats = []
posits = []
with open(filename + "_double.txt") as f:
    for line in f:
        # Note that float in python gives us a double, which should be good enough
        doubles.append(Decimal(float(line)))
with open(filename + "_float.txt") as f:
    for line in f:
        floats.append(Decimal(float(line)))
with open(filename + "_posit.txt") as f:
    for line in f:
        posits.append(Decimal(float(line)))
with open(filename + "_float_error.txt", "w") as f:
    for i in range(0, min(len(doubles), len(floats))):
        f.write(str(abs((doubles[i] - floats[i]) / doubles[i])) + "\n")
with open(filename + "_posit_error.txt", "w") as f:
    for i in range(0, min(len(doubles), len(floats))):
        f.write(str(abs((doubles[i] - posits[i]) / doubles[i])) + "\n")
