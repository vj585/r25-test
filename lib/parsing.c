#include <stdint.h>
#include <parsing.h>
#include <math.h>
// implement interpolation here
int interpolation(uint16_t channel) {
	// write interpolation code here
    const uint16_t INPUT_MIN = 0;        // Minimum input value (typically 0)
    const uint16_t INPUT_MAX = 2047;     // Maximum input value for 11-bit channel
    const int OUTPUT_MIN = 0;            // Minimum output value for Sabertooth
    const int OUTPUT_MAX = 254;          // Maximum output value for Sabertooth

    // Scale channel value from input range to output range
    return (channel - INPUT_MIN) * (OUTPUT_MAX - OUTPUT_MIN) / (INPUT_MAX - INPUT_MIN) + OUTPUT_MIN;
}
// creating 11 bit channel
uint16_t *parse_buffer(uint8_t buff[]) { 
		
	// to store channels
	static uint16_t channel[16];

	// masking byte shiftings bits (value in hexa '0x07FF')
	uint16_t mask = 0x7ff;

	// creating channels 
        channel[0]  = ((buff[1] | buff[2]<<8)                 & mask);
        channel[1]  = ((buff[2]>>3 | buff[3]<<5)              & mask);
        channel[2]  = ((buff[3]>>6 | buff[4]<<2 | buff[5]<<10) & mask);
        channel[3]  = ((buff[5]>>1 | buff[6]<<7)              & mask);
        channel[4]  = ((buff[6]>>4 | buff[7]<<4)              & mask);
        channel[5]  = ((buff[7]>>7 | buff[8]<<1 | buff[9]<<9)  & mask);
        channel[6]  = ((buff[9]>>2 | buff[10]<<6)             & mask);
        channel[7]  = ((buff[10]>>5| buff[11]<<3)             & mask);
        channel[8]  = ((buff[12]   | buff[13]<<8)             & mask);
        channel[9]  = ((buff[13]>>3| buff[14]<<5)             & mask);
        channel[10] = ((buff[14]>>6| buff[15]<<2| buff[16]<<10)& mask);
        channel[11] = ((buff[16]>>1| buff[17]<<7)             & mask);
        channel[12] = ((buff[17]>>4| buff[18]<<4)             & mask);
        channel[13] = ((buff[18]>>7| buff[19]<<1| buff[20]<<9) & mask);
        channel[14] = ((buff[20]>>2| buff[21]<<6)             & mask);
        channel[15] = ((buff[21]>>5| buff[22]<<3)             & mask);

	return channel;
}


