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
            self.data_object.append(
                DataObject(
                    start_delimeter=hex_string[0:2],
                    length=hex_string[2:6],
                    frame_type=hex_string[6:2],
                    sixtyfour_address=hex_string[6:8],
                    sixteen_address=hex_string[0:2],
                    receive_options=hex_string[0:2],
                    number_samples=hex_string[0:2],
                    digital_channel_mask=hex_string[0:2],
                    analog_channel_mask=hex_string[0:2],
                    analog_value=hex_string[0:2],
                    checksum=hex_string[0:2]
                )
            )
            
            break
        
        for data in self.data_object:
            print(data)