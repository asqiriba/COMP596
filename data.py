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
        Length: {self.length}
        Frame type: {self.frame_type} (IO Data Sample RX Indicator)
        64-bit source address: {self.sixtyfour_address}
        16-bit source address: {self.sixteen_address}
        Receive options: {self.receive_options}
        Number of samples: {self.number_samples}
        Digital channel mask: {self.digital_channel_mask}
        Analog channel mask: {self.analog_channel_mask}
        DIO3/AD3 analog value: {self.analog_value}
        Checksum: {self.checksum}
        """