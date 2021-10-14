"""
"""

from cryptography.fernet import Fernet


class Encrypt:
    def __init__(self, key, message) -> None:
        self.key = key
        self.message = message

    def _data_process(self, list, mode):
        coded_list = []

        if(mode == "e"):
            for element in list:
                coded_list.append(element.encode())
        else:
            for element in list:
                coded_list.append(element.decode())
        
        return coded_list

    def _data_encryption(self, algorithm, list):
        encrypted_list = []

        for element in list:
            encrypted_list.append(algorithm.encrypt(element))
        
        return encrypted_list

    def _data_decryption(self, algorithm, list):
        decrypted_list = []

        for element in list:
            decrypted_list.append(algorithm.decrypt(element))
        
        return decrypted_list

    def ingest(self):
        algorithm = Fernet(self.key)
        clean_messages = self._data_process(list=self.message, mode="e")
        encrypted_messages = self._data_encryption(algorithm=algorithm, list=clean_messages)

        return encrypted_messages

    def digest(self):
        algorithm = Fernet(self.key)
        decrypted_messages = self._data_decryption(algorithm=algorithm, list=self.message)
        clean_messages = self._data_process(list=decrypted_messages, mode="d")

        return clean_messages