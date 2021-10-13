"""
"""
import os.path

class Parser:
    def __init__(self, path) -> None:
        self.packets = []
        self.digital_outputs = []

        self._header = "Digi XBee3 Zigbee"
        self.path = path

    def read_packets(self):
        file_exists = os.path.exists(self.path)

        if(not file_exists):
            print("File not found. Check path.")
            return

        with open(self.path) as file:
            for line in file:
                if(self._header in line or line == "\n"):
                    continue

                self.packets.append(line.split(","))
        
        self._get_hex()
        return self.digital_outputs
    
    def _get_hex(self):
        for date, index, api, hex in self.packets:
            self.digital_outputs.append(hex)

    def _get_api(self):
        for date, index, api, hex in self.packets:
            self.digital_outputs.append(api)
    
    def _get_index(self):
        for date, index, api, hex in self.packets:
            self.digital_outputs.append(index)
    
    def _get_date(self):
        for date, index, api, hex in self.packets:
            self.digital_outputs.append(date)

if(__name__ == "__main__"):
    pass