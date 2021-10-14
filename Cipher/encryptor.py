"""
"""

from cryptography.fernet import Fernet


class Encrypt:
    def __init__(self, key, message) -> None:
        self.key = key
        self.message = message

    def _data_process(self, list):
        encoded_list = []

        for element in list:
            encoded_list.append(element.encode())
        
        return encoded_list

    def _data_encryption(self, algorithm, list):
        encrypted_list = []

        for element in list:
            encrypted_list.append(algorithm.encrypt(element))
        
        return encrypted_list

    def ingest(self):
        algorithm = Fernet(self.key)
        clean_messages = self._data_process(list=self.message)
        encrypted_messages = self._data_encryption(algorithm=algorithm, list=clean_messages)

        return encrypted_messages