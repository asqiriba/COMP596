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
                    start_delimeter=hex_string[0:2]
                )
            )
            
            break
        
        for data in self.data_object:
            print(data)