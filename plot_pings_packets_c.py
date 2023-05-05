import re
import matplotlib.pyplot as plt

# read the data from the file
with open("pings_results_c.txt", "r") as f:
    data = f.readlines()

# parse the data and calculate the time needed to send each packet
times = []
for line in data:
    match = re.search(r'^\d+\s+(\d+\.\d+)$', line)
    if match:
        time_str = match.group(1)
        times.append(float(time_str))

# plot the graph
plt.plot(times, range(len(times)))
plt.xlabel("Time needed to send a packet (seconds)", fontsize=12)
plt.ylabel("Packet number", fontsize=12)
plt.title("Packet number vs Time needed to send a packet")
plt.savefig("Pings_c.png")

print("Plot saved to Syn_pkts_p.png")
