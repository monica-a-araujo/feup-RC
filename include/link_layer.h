// Link layer header.
// NOTE: This file must not be changed.

#ifndef _LINK_LAYER_H_
#define _LINK_LAYER_H_

typedef enum
{
    LlTx,
    LlRx,
} LinkLayerRole;

typedef struct
{
    char serialPort[50];
    LinkLayerRole role;
    int baudRate;
    int nRetransmissions;
    int timeout;
} LinkLayer;

// Send data in buf with size bufSize.
// Return number of chars written, or "-1" on error.
int llwrite(int fd, unsigned char *buf, int bufSize);

// SIZE of maximum acceptable payload.
// Maximum number of bytes that application layer should send to link layer
#define MAX_PAYLOAD_SIZE 1000

// MISC
#define FALSE 0
#define TRUE 1

// Open a connection using the "port" parameters defined in struct linkLayer.
// Return "1" on success or "-1" on error.
int llopen(LinkLayer connectionParameters);

// Receive data in packet.
// Return number of chars read, or "-1" on error.
int llread(unsigned char *packet, int fd);

// Close previously opened connection.
// if showStatistics == TRUE, link layer should print statistics in the console on close.
// Return "1" on success or "-1" on error.
int llclose(int showStatistics, int fd, int role);


void handle_alarm_timeout();
void handle_alarm_timeout();
void alarm_off();
int frame_i(char *data, char *frame, int data_len, char CMD);
int stuffing(char * frame, int* fr_len);
int destuffing(char * frame, int* fr_len);
int closefd(int fd, struct termios* oldtio);
void turnOffAlarm();
int frame_i_generator(char *data, char *frame, int data_len, char controlField);
void install_alarm();
void changeState(char buffer, int *state);

int readframe_NS_A(int fd, char controlField);
int readframe_S_A(int fd, char *controlField);
int sendframe_S_U(int fd, char addressField, char controlField);
