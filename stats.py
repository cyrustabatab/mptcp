import glob
import numpy as np

file_list = glob.glob("subflow_*.csv")

stats_file = open("stats.txt", "w")
for f in file_list:
        stats_file.write(f + "\n")
	array  = np.genfromtxt(f)
        stats_file.write("Average: " + str(np.mean(array))+ "\n");
        stats_file.write("Standard Deviation: " + str(np.std(array)) + "\n\n")
stats_file.close()
