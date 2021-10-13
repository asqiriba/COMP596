"""
"""

packets = []
digital_outputs = []

header = "Digi XBee3 Zigbee"
path = "COMP596/api_console_session_2021-10-12.log"

with open(path) as file:
    for line in file:
        if(header in line or line == "\n"):
            continue
        packets.append(line)
