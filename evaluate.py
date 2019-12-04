import sys
from statistics import mean, stdev
filename = "fpbench_results/" + sys.argv[1]
float_results = []
with open(filename + "_float_error.txt") as f:
    for line in f:
        float_results.append(float(line))
print("Length: " + str(len(float_results)))
print("Float mean error:\t" + str(mean(float_results)))
print("Float error stdev:\t" + str(stdev(float_results)))
posit_results = []
with open(filename + "_posit_error.txt") as f:
    for line in f:
        posit_results.append(float(line))
print("Posit mean error:\t" + str(mean(posit_results)))
print("Posit error stdev:\t" + str(stdev(posit_results)))
posit_gain = [float_results[i] - posit_results[i] for i in range(len(float_results))]
print("Mean Posit - Float:\t" + str(mean(posit_gain)))
print("Stdev Posit - Float:\t" + str(stdev(posit_gain)))
