import glob
import numpy as np

file_list = glob.glob("SF_*.csv")
rtt_list = glob.glob("RTT_*.csv")
stats_file = open("stats.txt", "w")
for f,r in zip(file_list,rtt_list):
        stats_file.write(f + "\n")
	array  = np.genfromtxt(f)
        rtt_array = np.genfromtxt(r)
        stats_file.write("CWND Average: " + str(np.mean(array))+ "\n");
        stats_file.write("CWND Standard Deviation: " + str(np.std(array)) + "\n")
        stats_file.write("RTT Average: " + str(np.mean(rtt_array)) + "\n");
        stats_file.write("RTT Standard Deviation: " + str(np.std(rtt_array)) + "\n\n");
        
stats_file.close()
