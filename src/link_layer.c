// Link layer protocol implementation

#include "link_layer.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{   
    int fd;
    int connection = -1; //conecão ainda não esta feita


    //erro - tipo de conecção errada
    if (connectionParameters.role != LlTx && connectionParameters.role != LlRx){
        printf("Actual flag %d. MUst be %d or %d", connectionParameters.role, LlTx, LlRx);
        return -1;
    }

    //Coneçao com o emissor
    if(connectionParameters.role==LlTx){

        fd = openfd(connectionParameters.serialPort, connectionParameters.baudRate);
        

        while (connection != 0){
        //iniciar alarme
        //mandar set (verificar que se mandou mesmo se não erro)
        //ler UA (verificar que se mandou o certo, se não erro)

       }
    } 

    //coneção com o receptor
    else if(connectionParameters.role == LlRx){

    }

    return 1;
}
int openfd(char serialPort[50],int baudRate){
    struct termios oldtio;
    struct termios newtio;

    // Open serial port device for writing and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    int fd = open(serialPort, O_WRONLY | O_NOCTTY);
    if (fd < 0)
    {
        perror(serialPort);
        exit(-1);
    }


    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    // BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
    // CRTSCTS : output hardware flow control
    //           ATTENTION:  only used if the cable has all necessary lines (is
    //           a null modem cable), otherwise the output flow control is set
    //           automatically by software by the driver.
    // CS8     : 8n1 (8bit,no parity,1 stopbit)
    // CLOCAL  : local connection, no modem control
    // CREAD   : enable receiving characters
    newtio.c_cflag = baudRate | CS8 | CLOCAL | CREAD;

    // IGNPAR: Ignore framing and parity errors
    // ICRNL:  Map CR to NL (otherwise a CR input on the other computer
    //         will not terminate input)
    // Otherwise make device raw (no other input processing)
    newtio.c_iflag = IGNPAR | ICRNL;

    // Raw output
    newtio.c_oflag = 0;

    // ICANON : enable canonical input
    // disable all echo functionality, and don't send signals to calling program
    newtio.c_lflag = ICANON;

    // Initialize all control characters
    // default values can be found in /usr/include/termios.h, and are given
    // in the comments, but we don't need them here
    newtio.c_cc[VINTR] = 0;    // Ctrl-c
    newtio.c_cc[VQUIT] = 0;    // Ctrl-'\'
    newtio.c_cc[VERASE] = 0;   // del
    newtio.c_cc[VKILL] = 0;    // @
    newtio.c_cc[VEOF] = 4;     // Ctrl-d
    newtio.c_cc[VTIME] = 0;    // inter-character timer unused
    newtio.c_cc[VMIN] = 1;     // blocking read until 1 character arrives
    newtio.c_cc[VSWTC] = 0;    // '\0'
    newtio.c_cc[VSTART] = 0;   // Ctrl-q
    newtio.c_cc[VSTOP] = 0;    // Ctrl-s
    newtio.c_cc[VSUSP] = 0;    // Ctrl-z
    newtio.c_cc[VEOL] = 0;     // '\0'
    newtio.c_cc[VREPRINT] = 0; // Ctrl-r
    newtio.c_cc[VDISCARD] = 0; // Ctrl-u
    newtio.c_cc[VWERASE] = 0;  // Ctrl-w
    newtio.c_cc[VLNEXT] = 0;   // Ctrl-v
    newtio.c_cc[VEOL2] = 0;    // '\0'

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred  to
    // by  fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    return fd;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(const unsigned char *buf, int bufSize)
{
    // TODO

    return 0;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    // TODO

    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics)
{
    // TODO

    return 1;
}
