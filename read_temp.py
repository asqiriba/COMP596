"""
"""

from data_cleanup import DataCU
from log_parser import Parser

path = "COMP596/api_console_session_2021-10-12.log"

if(__name__ == "__main__"):
    parser = Parser(path=path)
    cleaner = DataCU(hex_values=parser.read_packets())