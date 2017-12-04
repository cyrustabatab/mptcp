import glob
import numpy as np

file_list = glob.glob("SF_*.csv")
rtt_list = glob.glob("RTT_*.csv")
stats_file = open("stats.txt", "w")
total_cwnds = []
total_rtts = []
max_cwnds = []
min_cwnds = []
max_rtts = []
min_rtts = []
for f,r in zip(file_list,rtt_list):
        stats_file.write(f + "\n")
	array  = np.genfromtxt(f)
        rtt_array = np.genfromtxt(r)
        total_cwnds.append(np.mean(array))
        max_cwnd = np.max(array)
        max_cwnds.append(max_cwnd) 
        min_cwnd = np.min(array)
        min_cwnds.append(min_cwnd)
        total_rtts.append(np.mean(rtt_array))
        max_rtt = np.max(rtt_array)
        max_rtts.append(max_rtt)
        min_rtt = np.min(rtt_array)
        min_rtts.append(min_rtt)
        stats_file.write("CWND Average: " + str(np.mean(array))+ "\n")
        stats_file.write("CWND Standard Deviation: " + str(np.std(array)) + "\n")
        stats_file.write("Max CWND: " + str(max_cwnd) + "\n")
        stats_file.write("Min CWND: " + str(min_cwnd) + "\n")
        stats_file.write("RTT Average: " + str(np.mean(rtt_array)) + "\n")
        stats_file.write("RTT Standard Deviation: " + str(np.std(rtt_array)) + "\n")
        stats_file.write("Max RTT: " + str(max_rtt) + "\n")
        stats_file.write("Min RTT: " + str(min_rtt) + "\n\n")
        

connection_cwnd = np.array(total_cwnds)
connection_rtt = np.array(total_rtts)
connection_max_cwnd = np.array(max_cwnds)
connection_min_cwnd = np.array(min_cwnds)
connection_max_rtt = np.array(max_rtts)
connection_min_rtt = np.array(min_rtts)

stats_file.write("Connection CWND Average: " + str(np.mean(connection_cwnd)) + "\n")
stats_file.write("Connection CWND Standard Deviation: " + str(np.std(connection_cwnd)) + "\n")
stats_file.write("Connection MAX CWND: " + str(np.max(connection_max_cwnd)) + "\n")
stats_file.write("Connection MIN CWND: " + str(np.min(connection_min_cwnd)) + "\n")
stats_file.write("Connection RTT Average: " + str(np.mean(connection_rtt)) + "\n")
stats_file.write("Connection RTT Standard Deviation: " + str(np.std(connection_rtt)) + "\n")
stats_file.write("Connection MAX RTT: " + str(np.max(connection_max_rtt)) + "\n")
stats_file.write("Connection MIN RTT: " + str(np.min(connection_min_rtt)) + "\n")

stats_file.close()
