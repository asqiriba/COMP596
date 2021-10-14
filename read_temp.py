"""
"""

from Services.data_cleanup import DataCU
from Services.log_parser import Parser
from Cipher.generator import KeyGen

PATH = "COMP596/Data/api_console_session_2021-10-12.log"

if(__name__ == "__main__"):
    parser = Parser(path=PATH)

    cleaner = DataCU(hex_values=parser.read_packets())
    clean_data = cleaner.clean_values()

    key = KeyGen()
    key.write_key()

    temperatures = []

    for data in clean_data:
        temperatures.append(data.analog_value)

    k = key.read_key()
    print(k)