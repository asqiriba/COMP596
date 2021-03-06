/**
 * sudo minicom --device /dev/ttyACM0
 * gcc xbee_temp_relay.c -lmraa -o temp_relay
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "present.h"
#include "mraa_beaglebone_pinmap.h"

#define LCD_ADDR 0x3e // I2C address of the LCD.

mraa_gpio_context Relay_pin;
mraa_i2c_context i2cp;

typedef struct __attribute__((__packed__)) byte
{
	uint8_t nibble1 : 4;
	uint8_t nibble2 : 4;
} byte;
// define the SBox
uint8_t S[] = {0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2};
// define the inverse SBox
uint8_t invS[] = {0x5, 0xe, 0xf, 0x8, 0xC, 0x1, 0x2, 0xD, 0xB, 0x4, 0x6, 0x3, 0x0, 0x7, 0x9, 0xA};
// define the permutation table
uint8_t P[] = {0, 16, 32, 48, 1, 17, 33, 49, 2, 18, 34, 50, 3, 19, 35, 51,
			   4, 20, 36, 52, 5, 21, 37, 53, 6, 22, 38, 54, 7, 23, 39, 55,
			   8, 24, 40, 56, 9, 25, 41, 57, 10, 26, 42, 58, 11, 27, 43, 59,
			   12, 28, 44, 60, 13, 29, 45, 61, 14, 30, 46, 62, 15, 31, 47, 63};
// function for converting a Hex String to an array of bytes
byte *fromHexStringToBytes(char *block)
{
	byte *bytes = malloc(8 * sizeof(byte));
	int i;
	// each character of the String is one nibble (4 bits), there are 8 bytes in a 64-bit block
	for (i = 0; i < 8; i++)
	{
		bytes[i].nibble1 = (block[2 * i] >= '0' && block[2 * i] <= '9') ? (block[2 * i] - '0') : (block[2 * i] - 'a' + 10);
		bytes[i].nibble2 = (block[2 * i + 1] >= '0' && block[2 * i + 1] <= '9') ? (block[2 * i + 1] - '0') : (block[2 * i + 1] - 'a' + 10);
	}
	return bytes;
}

void home_LCD(void)
{
	uint8_t buf[2] = {0x00, 0x02};
	mraa_i2c_write(i2cp, buf, 2); //Set to Home
}

void LCD_Print(char *str)
{
	uint8_t buf[80];

	int32_t i = 0, strl, j = 0;

	buf[i] = 0x40; //register for display
	strl = strlen((char *)str);
	for (j = 0; j < strl; j++)
	{
		i++;
		buf[i] = str[j];
	}

	mraa_i2c_write(i2cp, buf, i + 1);
}

void LCD_init(void)
{
	uint8_t init1[2] = {0x00, 0x38};
	uint8_t init2[8] = {0x00, 0x39, 0x14, 0x74, 0x54, 0x6f, 0x0c, 0x01};
	// 2 lines 8 bit 3.3V Version
	mraa_i2c_write(i2cp, init1, 2);
	mraa_i2c_write(i2cp, init2, 8); //Function Set
}

void clear_LCD(void)
{
	uint8_t buf[2] = {0x00, 0x01};

	mraa_i2c_write(i2cp, buf, 2); //Clear Display
}

void nextline_LCD(void)
{
	uint8_t buf[2] = {0x00, 0xC0};

	mraa_i2c_write(i2cp, buf, 2); //2nd line of LCD Display
}

// function for converting an array of bytes to a 64-bit integer
uint64_t fromBytesToLong(byte *bytes)
{
	uint64_t result = 0;
	int i;
	// multiplication with 16 replaced with shifting right 4 times
	// addition replaced with bitwise OR, since one of the operands is always 0
	for (i = 0; i < 8; i++)
	{
		result = (result << 4) | (bytes[i].nibble1 & 0xFUL);
		result = (result << 4) | (bytes[i].nibble2 & 0xFUL);
	}
	return result;
}
// function for converting Hex String to a 64-bit integer
uint64_t fromHexStringToLong(char *block)
{
	uint64_t result;
	int i;
	// each character is 4 bits, there are 16 characters in a 64-bit block
	// the multiplication and addition are done the same way as before, with shifting and bitwise OR
	for (i = 0; i < 16; i++)
		result = (result << 4) | ((block[i] >= '0' && block[i] <= '9') ? (block[i] - '0') : (block[i] - 'a' + 10));
	return result;
}
// function for converting a 64-bit integer to an array of bytes
byte *fromLongToBytes(uint64_t block)
{
	byte *bytes = malloc(8 * sizeof(byte));
	int i;
	// the nibbles for each byte are obtained by shifting the number to the right for appropriate number of places (a multiple of 4)
	// each nibble is obtained after masking the bits by performing bitwise AND with 1111 (all bits except the least significant 4 become 0)
	for (i = 7; i >= 0; i--)
	{
		bytes[i].nibble2 = (block >> 2 * (7 - i) * 4) & 0xFLL;
		bytes[i].nibble1 = (block >> (2 * (7 - i) + 1) * 4) & 0xFLL;
	}
	return bytes;
}

char *itoa(int val, int base)
{
	static char buf[32] = {0};

	int i = 30;

	for (; val && i; --i, val /= base)

		buf[i] = "0123456789abcdef"[val % base];

	return &buf[i + 1];
}

// function for converting a 64-bit integer to a Hex String
char *fromLongToHexString(uint64_t block)
{
	char *hexString = malloc(17 * sizeof(char));
	//we print the integer in a String in hexadecimal format
	sprintf(hexString, "%016lui", block);
	// printf("%s\n", itoa(hexString, 10));
	return hexString;
}
// function for converting a nibble using the SBox
uint8_t Sbox(uint8_t input)
{
	return S[input];
}
// inverse function of the one above (used to obtain the original nibble)
uint8_t inverseSbox(uint8_t input)
{
	return invS[input];
}
/*
    Function that performs the permutation according to the permutation table P
    The permutation is done by adding one bit at a time from the source block to the appropriate location in the result.
    In each iteration the following is performed:
    1) calculate the distance of the bit that is supposed to be added next from the least significant bit (at position 63)
    2) shift the source block to the right so that the bit becomes the least significant bit
    3) separate this bit by masking by performing bitwise and with 1
    4) calculate the new location (from right to left) of the bit (distance between the least significant bit and P[i])
    5) shift the bit to the new location and add it to the permutation using bitwise OR
*/
uint64_t permute(uint64_t source)
{
	uint64_t permutation = 0;
	int i;
	for (i = 0; i < 64; i++)
	{
		int distance = 63 - i;
		permutation = permutation | ((source >> distance & 0x1) << (63 - P[i]));
	}
	return permutation;
}
/*
    Function that performs the inverse permutation according to the table P
    Again, the permutation is done by adding one bit at a time to the result.
    In each iteration the following is performed:
    1) calculate the position of the bit that should be on the i-th position
    2) shift the result 1 bit to the left (i-th bit of the final permutation is on the least significant location)
    3) shift the source block so that the needed bit comes to the least significant location
    4) separate this bit by masking by performing bitwise and with 1
    5) add it to the (inverse)permutation using bitwise OR
*/
uint64_t inversepermute(uint64_t source)
{
	uint64_t permutation = 0;
	int i;
	for (i = 0; i < 64; i++)
	{
		int distance = 63 - P[i];
		permutation = (permutation << 1) | ((source >> distance) & 0x1);
	}
	return permutation;
}
// function that returns the low 16 bits of the key, which is given as input in a Hex String format
uint16_t getKeyLow(char *key)
{
	int i;
	uint16_t keyLow = 0;
	//the least significant 16 bits are the last 4 characters of the key
	for (i = 16; i < 20; i++)
		//again, multiplication and addition are done using bitwise left shift and bitwise OR
		keyLow = (keyLow << 4) | (((key[i] >= '0' && key[i] <= '9') ? (key[i] - '0') : (key[i] - 'a' + 10)) & 0xF);
	return keyLow;
}
// function that generates subKeys from the key according to the PRESENT key scheduling algorithm for a 80-bit key
uint64_t *generateSubkeys(char *key)
{
	//the 80 bit key is placed in two integers, one that is 16-bit (keyLow) and the other one is 64-bit (keyHigh)
	uint64_t keyHigh = fromHexStringToLong(key);
	uint16_t keyLow = getKeyLow(key);
	//we allocate space for 32 subkeys, since there are 32 rounds
	uint64_t *subKeys = malloc(32 * (sizeof(uint64_t)));
	int i;
	//the first subkey is the high part of the original key
	subKeys[0] = keyHigh;
	for (i = 1; i < 32; i++)
	{
		//shifting the whole key (high and low) 61 bits to the left (temporary variables needed to preserve data
		uint64_t temp1 = keyHigh, temp2 = keyLow;
		keyHigh = (keyHigh << 61) | (temp2 << 45) | (temp1 >> 19);
		keyLow = ((temp1 >> 3) & 0xFFFF);
		//the most significant nibble of the key goes through the SBox
		uint8_t temp = Sbox(keyHigh >> 60);
		//the old value of the most significant nibble is set to zero using masking
		keyHigh = keyHigh & 0x0FFFFFFFFFFFFFFFLL;
		//new most significant nibble (output of the SBox) is placed on the most significant location
		keyHigh = keyHigh | (((uint64_t)temp) << 60);
		//key bits on positions k19, k18, k17, k16 and k15 XORed with round counter
		keyLow = keyLow ^ ((i & 0x01) << 15); //k15 is the most significant bit in keyLow
		keyHigh = keyHigh ^ (i >> 1);		  //the other bits are the least significant ones in keyHigh
		//according to the key scheduling algorithm, the values of keyHigh are used as 64-bit subkeys
		subKeys[i] = keyHigh;
	}
	return subKeys;
}
// function for encrypting a block using a key
char *encrypt(char *plaintext, char *key)
{
	//generate the subkeys using the function defined above
	uint64_t *subkeys = generateSubkeys(key);
	//convert the plaintext from a Hex String to a 64-bit integer
	uint64_t state = fromHexStringToLong(plaintext);
	int i, j;
	//apply first 31 rounds
	for (i = 0; i < 31; i++)
	{
		//XOR the state with the round subkey
		state = state ^ subkeys[i];
		//convert the state from a 64-bit integer to an array of bytes (nibbles)
		byte *stateBytes = fromLongToBytes(state);
		//run each nibble through the SBox
		for (j = 0; j < 8; j++)
		{
			stateBytes[j].nibble1 = Sbox(stateBytes[j].nibble1);
			stateBytes[j].nibble2 = Sbox(stateBytes[j].nibble2);
		}
		//return the nibbles in a 64-bit integer format and perform the permutation defined above
		state = permute(fromBytesToLong(stateBytes));
		//free the memory of the byte array (not needed anymore)
		free(stateBytes);
	}
	//the last round only XORs the state with the round key
	state = state ^ subkeys[31];
	//free the memory of the subkeys (they are not needed anymore)
	free(subkeys);

	return fromLongToHexString(state);
}
// function for decrypting a block using a key
char *decrypt(char *ciphertext, char *key)
{
	//generate the subkeys using the function defined above
	uint64_t *subkeys = generateSubkeys(key);
	//convert the plaintext from a Hex String to a 64-bit integer
	uint64_t state = fromHexStringToLong(ciphertext);
	int i, j;
	//apply first 31 rounds
	for (i = 0; i < 31; i++)
	{
		//XOR the state with the round subkey (in decryption, order of subkeys is inversed)
		state = state ^ subkeys[31 - i];
		//perform the inverse permutation defined above
		state = inversepermute(state);
		//convert the state from a 64-bit integer to an array of bytes (nibbles)
		byte *stateBytes = fromLongToBytes(state);
		//run each nibble through the inverse SBox
		for (j = 0; j < 8; j++)
		{
			stateBytes[j].nibble1 = inverseSbox(stateBytes[j].nibble1);
			stateBytes[j].nibble2 = inverseSbox(stateBytes[j].nibble2);
		}
		//return the nibbles in a 64-bit integer format
		state = fromBytesToLong(stateBytes);
		//free the memory of the byte array (not needed anymore)
		free(stateBytes);
	}
	//the last round only XORs the state with the round key
	state = state ^ subkeys[0];
	//free the memory of the subkeys (they are not needed anymore)
	free(subkeys);

	return fromLongToHexString(state);
}

unsigned int threshold = 90;

int main(int argc, char **argv)
// int main()
{
	char buffer[1024], buf_Xbee[1024];
	unsigned int ADC = 0, timeout = 2000;
	double Temp = 0, V36 = 0;
	char *key = malloc(21 * sizeof(char));
	char *plaintext, *ciphertext, *deciphertext;
	char dev_string[] = "/dev/ttyS1";

	mraa_init();

	key = "66546A576E5A7234";

	i2cp = mraa_i2c_init_raw(I2CP_BUS);
	mraa_i2c_frequency(i2cp, MRAA_I2C_STD);
	mraa_i2c_address(i2cp, LCD_ADDR);

	Relay_pin = mraa_gpio_init(RELAY_PIN);
	mraa_gpio_dir(Relay_pin, MRAA_GPIO_OUT);

	mraa_uart_context uart;
	mraa_boolean_t isDataReady;
	uart = mraa_uart_init_raw(dev_string);
	mraa_uart_set_baudrate(uart, 9600);
	mraa_uart_set_mode(uart, 8, MRAA_UART_PARITY_NONE, 1);
	mraa_uart_set_flowcontrol(uart, 0, 0);
	mraa_uart_set_timeout(uart, 0, 0, 0);

	if (uart == NULL)
	{
		printf("UART failed to setup\n");
		return 0;
	}

	LCD_init();
	clear_LCD();
	home_LCD();
	LCD_Print((char *)("ASCIIriba"));
	sleep(1);

	mraa_gpio_write(Relay_pin, 0);

	while (1)
	{
		isDataReady = mraa_uart_data_available(uart, timeout);

		if (isDataReady == 1)
		{
			mraa_uart_read(uart, buf_Xbee, sizeof(buffer));

			ADC = (buf_Xbee[21] & 0x3) << 8 | buf_Xbee[22];
			V36 = (ADC * 1250) / 1023;
			Temp = (V36 - 500) / 10;

			printf("Temp:%3.3f'C, threshold:%u'C\n", Temp, threshold);

			sprintf(buffer, "Sensor:%3.1f'C", Temp);

			clear_LCD();
			home_LCD();
			LCD_Print(buffer);

			nextline_LCD();

			if (Temp > threshold)
			{
				sprintf(buffer, "Relay On ");
				LCD_Print(buffer);
				mraa_gpio_write(Relay_pin, 0);
				sleep(1);
			}
			else
			{
				ciphertext = encrypt(plaintext, key);
				deciphertext = decrypt(ciphertext, key);

				sprintf(buffer, "Cypher:%u'C\n", ciphertext); // Should this be %u?
				LCD_Print(buffer);
				mraa_gpio_write(Relay_pin, 1);
				printf("Decipher_text_hex: %x\n", deciphertext);
				sleep(1);
			}
		}
		else
		{
			printf("Error: Timeout. UART data not retrieved after %u ms\n", timeout);
		}
	}

	//freeing the allocated memory
	free(ciphertext);
	free(deciphertext);

	return 0;
}
