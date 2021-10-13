"""
"""

from data import DataObject


class DataCU:
    def __init__(self, hex_values) -> None:
        self.raw_data = hex_values
        self.data_object = []

        self._split_values()
    
    def _split_values(self):
        for hex_string in self.raw_data:
            if(len(hex_string) != 45):
                continue

            self.data_object.append(
                DataObject(
                    start_delimeter=hex_string[0:2],
                    length=hex_string[2:6],
                    frame_type=hex_string[6:8],
                    sixtyfour_address=hex_string[8:24],
                    sixteen_address=hex_string[24:28],
                    receive_options=hex_string[28:30],
                    number_samples=hex_string[30:32],
                    digital_channel_mask=hex_string[32:36],
                    analog_channel_mask=hex_string[36:38],
                    analog_value=hex_string[38:42],
                    checksum=hex_string[42:44]
                )
            )
        
        index = -1
        for data in self.data_object:
            index += 1
            print(index, data)