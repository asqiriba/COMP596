from cryptography.fernet import Fernet

class KeyGen:
    _PATH = "COMP596/Cipher/pass.key"

    def write_key(self):
        key = Fernet.generate_key()

        with open(self._PATH, "wb") as key_file:
            key_file.write(key)

        print(f"\n\n\nKey generated in: {self._PATH}\n\n\n")

    def read_key(self):
        try:
            f = open(self._PATH, 'rb')

        except OSError:
            print("Could not read key.")
            return

        with f:
            key = ""

            for line in f:
                key = line

            print(f"\n\n\nKey read: {f}\n\n\n")

            return key