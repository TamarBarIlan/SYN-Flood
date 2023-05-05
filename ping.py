import os
import time
import matplotlib.pyplot as plt

# IP address to ping
ip_address = "10.9.0.5"

# Ping command
ping_cmd = f"ping -c 1 {ip_address}"

rtt_values = []

# Serial number counter
serial_num = 1

# Ping loop
while True:
    try:
        # Execute ping command and get output
        ping_output = os.popen(ping_cmd).read()

        # Extract round trip time (RTT) from ping output
        rtt_pos = ping_output.find("time=")
        if rtt_pos != -1:
            rtt = ping_output[rtt_pos:].split()[0][5:]
        else:
            rtt = "Timeout"

        # Add RTT to list
        rtt_values.append(float(rtt))

        # Write serial number and RTT to file
        with open('pings_results_c.txt', 'a') as f:
            f.write(f"{serial_num} {rtt}\n")

        # Increment serial number
        serial_num += 1

        # Wait 5 seconds
        time.sleep(5)

    except KeyboardInterrupt:
        # If the user enters Ctrl+C, break out of the loop
        break

# Calculate and write average RTT to file
if len(rtt_values) > 0:
    avg_rtt = sum(rtt_values) / len(rtt_values)
    with open('pings_results_c.txt', 'a') as f:
        f.write(f"Average RTT: {avg_rtt} ms\n")