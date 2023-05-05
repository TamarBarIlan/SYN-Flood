import re
import matplotlib.pyplot as plt

# read the data from the file
with open("syns_results_p.txt", "r") as f:
    data = f.readlines()

# parse the data and calculate the time needed to send each packet
times = []
for line in data:
    match = re.search(r'^\d+\s+(\d+\.\d+)$', line)
    if match:
        time_str = match.group(1)
        times.append(float(time_str))

# plot the graph
plt.plot(times, range(1, len(times) + 1))
plt.xlabel("Time needed to send a packet (seconds)", fontsize=12)
plt.ylabel("Number of packets sent", fontsize=12)
plt.title("Number of packets sent vs Time needed to send a packet")
plt.savefig("Syn_pkts_p.png")

print("Plot saved to Syn_pkts_p.png")
