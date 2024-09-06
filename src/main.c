#include <stdint.h>
#include <stdio.h>
#include <serial.h>
#include <parsing.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <errno.h>
int open_file(const char *port_name, int baudrate);
void close_file(int fd);
int read_SBUS(uint8_t *buffer, int fd);
uint16_t* parse_buffer(uint8_t *sbus_packet);
int interpolation(uint16_t sbus_value);
void write_to_SB(int fd, int pwm_value);

int main(int argc, char** argv) {
if (argc < 3) {
        fprintf(stderr, "Usage: %s <sbus_port> <sabertooth_port>\n", argv[0]);
        return 1;
    }
	char *port_name_1 = argv[1]; // SBUS 
	char *port_name_2 = argv[2]; // Sabertooth1
int sbus_fd = open_file(port_name_1, 100000); // SBUS uses 100000 baud
    int sabertooth_fd = open_file(port_name_2, 9600); // Sabertooth is set to 9600 baud

    if (sbus_fd == -1 || sabertooth_fd == -1) {
        fprintf(stderr, "Error opening serial ports.\n");
        return 1;
    }
uint8_t sbus_packet[SBUS_PACKET_SIZE];
    uint16_t *channel_values;
while (1) {
        if (read_SBUS(sbus_packet, sbus_fd) == SBUS_PACKET_SIZE)
{
channel_values = parse_buffer(sbus_packet);
int pwm = interpolation(channel_values[0]);
write_to_SB(sabertooth_fd, pwm);
        }
    }
close_file(sbus_fd);
    close_file(sabertooth_fd);

    return 0;
}
int open_file(const char *port_name, int baudrate) {
    int fd = open(port_name, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror("Unable to open port");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    options.c_cflag |= (CLOCAL | CREAD);  // Enable the receiver and set local mode
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;  // 8 data bits
    options.c_cflag &= ~PARENB;  // No parity
    options.c_cflag &= ~CSTOPB;  // 1 stop bit
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}
void close_file(int fd) {
    close(fd);
}
int read_SBUS(uint8_t *buffer, int fd) {
    int total_bytes = 0;
    while (total_bytes < SBUS_PACKET_SIZE) {
        int bytes = read(fd, buffer + total_bytes, SBUS_PACKET_SIZE - total_bytes);
        if (bytes < 0) {
            perror("Error reading SBUS data");
            return -1;
        }
        total_bytes += bytes;
    }
    return total_bytes;
}
uint16_t* parse_buffer(uint8_t *sbus_packet){
    static uint16_t channels[16];
    channels[0]=((sbus_packet[1]      | sbus_packet[2] << 8)  & 0x07FF);
    channels[1]=((sbus_packet[2] >> 3 | sbus_packet[3] << 5)  & 0x07FF);
    channels[2]=((sbus_packet[3] >> 6 | sbus_packet[4] << 2 | sbus_packet[5] << 10) & 0x07FF);
    channels[3]=((sbus_packet[5] >> 1 | sbus_packet[6] << 7)  & 0x07FF);
    channels[4]=((sbus_packet[6] >> 4 | sbus_packet[7] << 4)  & 0x07FF);
    channels[5]=((sbus_packet[7] >> 7 | sbus_packet[8] << 1 | sbus_packet[9] << 9) & 0x07FF);
    channels[6]=((sbus_packet[9] >> 2 | sbus_packet[10] << 6) & 0x07FF);
    channels[7]=((sbus_packet[10] >> 5 | sbus_packet[11] << 3) & 0x07FF);
 return channels;
}
int interpolation(uint16_t sbus_value) {
    return (sbus_value * (PWM_MAX - PWM_MIN) / 2048) + PWM_MIN;
}
void write_to_SB(int fd, int pwm_value) {
    char buffer[2];
    buffer[0] = pwm_value; // Sabertooth accepts 1-127 for control
    write(fd, buffer, 1); // Write 1 byte command
}


	FILE *sbus; 
	FILE *sabertooth;

	// to store sbus packets
	uint8_t sbus_packet[15];

	// to store value of indiviual RC channel
	uint16_t *channel;

	// pwm value after interpolation 
	int pwm;

	// opening serial port for serial communication with Sabertooth and SBUS
	sbus = open_file(port_name_1, "rb");
	sabertooth = open_file(port_name_2, "w+");
	
	// read data from RC transmitter using sbus
	read_SBUS(sbus_packet, uint8_t, 25, sbus);

	// parsing sbus packet
	channel = parse_buffer(sbus_packet);

	// get pwm range for Sabertooth 1			 
	pwm = interpolation(channel[0]);		//  write								
							//  to
	// write data to Sabertooth 1			//  sabertooth	
	write_to_SB(sabertooth, "%d", pwm);		

	// closing all serial port 
	close_file(sbus);
	close_file(sabertooth);
}
