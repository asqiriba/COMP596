class DataObject:
    def __init__(
        self, 
        start_delimeter,
        length,
        frame_type,
        sixtyfour_address,
        sixteen_address,
        receive_options,
        number_samples,
        digital_channel_mask,
        analog_channel_mask,
        analog_value,
        checksum,
    ) -> None:

        self.start_delimeter = start_delimeter
        self.length = length
        self.frame_type = frame_type
        self.sixtyfour_address = sixtyfour_address
        self.sixteen_address = sixteen_address
        self.receive_options = receive_options
        self.number_samples = number_samples
        self.digital_channel_mask = digital_channel_mask
        self.analog_channel_mask = analog_channel_mask
        self.analog_value = analog_value
        self.checksum = checksum
    
    def __str__(self) -> str:
        return f"""
        Start delimiter: {self.start_delimeter}
        Length: 00 12 (18)
        Frame type: 92 (IO Data Sample RX Indicator)
        64-bit source address: 00 13 A2 00 41 CB FD FC
        16-bit source address: A7 CE
        Receive options: 01
        Number of samples: 01
        Digital channel mask: 00 00
        Analog channel mask: 08
        DIO3/AD3 analog value: 02 55 (597)
        Checksum: DD
        """