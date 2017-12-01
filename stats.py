import glob
import numpy as np

file_list = glob.glob("SF_*.csv")
rtt_list = glob.glob("RTT_*.csv")
stats_file = open("stats.txt", "w")
total_cwnds = []
total_rtts = []
for f,r in zip(file_list,rtt_list):
        stats_file.write(f + "\n")
	array  = np.genfromtxt(f)
        rtt_array = np.genfromtxt(r)
        total_cwnds.append(np.mean(array));
        total_rtts.append(np.mean(rtt_array));
        stats_file.write("CWND Average: " + str(np.mean(array))+ "\n");
        stats_file.write("CWND Standard Deviation: " + str(np.std(array)) + "\n")
        stats_file.write("RTT Average: " + str(np.mean(rtt_array)) + "\n");
        stats_file.write("RTT Standard Deviation: " + str(np.std(rtt_array)) + "\n\n");
        

connection_cwnd = np.array(total_cwnds)
connection_rtt = np.array(total_rtts)

stats_file.write("Connection CWND Average: " + str(np.mean(connection_cwnd)) + "\n")
stats_file.write("Connection CWND Standard Deviation: " + str(np.std(connection_cwnd)) + "\n")
stats_file.write("Connection RTT Average: " + str(np.mean(connection_rtt)) + "\n")
stats_file.write("Connection RTT Standard Deviation: " + str(np.std(connection_rtt)) + "\n")

stats_file.close()
