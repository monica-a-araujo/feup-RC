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
#include <variables.h>
#include <signal.h>

int numTransmissions = 0;
int fd_trans = 0;
struct termios oldtio_trans;
struct termios oldtio_rec;


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

int llwrite(int fd, const unsigned char *buf, int bufSize){
    static int sval_sen = 0;
    int fr_len;
    char CMD;

    char * fr = (char*) malloc(MAX_SIZE_ALLOC*sizeof(char));
    char * buf_cpy = (char*) malloc(MAX_SIZE_ALLOC*sizeof(char));

    if(bufSize < 0){
        printf("Invalid length : %d",bufSize);
        return -1;
    }

    memcpy(buf_cpy, buf,bufSize);

    for(int i = 0;TRUE;i++){
        memcpy(buf,buf_cpy,bufSize);
        memset(fr,0,strlen(fr));

        fr_len = frame_i(buf, fr, bufSize, (0x00 | (sval_sen << 6 )));
        alarm(TIMEOUT);
        if( write(fd,fr,fr_len) < 0){
            printf("Sender wasn't able to write into frame.")
            continue;
        }
        else printf("Frame sent with S = %d",sval_sen);

        if(r_fr_s(fd,&CMD)<0){
            printf("Wasn't able to read info frame.");
            continue;
        }
        else printf("Succeded to read CMD= %02x with R =%d ",CMD,!sval_sen);

        if ((CMD == 0x05 | ( 1 << 7 )) && sval_sen == 0 || (CMD == 0x05 | ( 0 << 7 )) && sval_sen == 1){
            alarm_off();
            sval_sen = !sval_sen;
            free(fr);
            free(buf_cpy);
            return 0;
        }
        if (CMD == (0x01 | ( !sval_sen << 7 )) || CMD == (0x01 | ( sval_sen<< 7 ))){
            alarm_off();
            printf("Error : Received REJ SIGNAL");
            continue;
        }
    }

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
int llclose(int showStatistics, int i)
{
    // TODO

    return 1;
}


void install_alarm() {
    if (signal(SIGALRM, handle_alarm_timeout) == SIG_ERR)
    {
        printf("It wasn't possible to install signal.");
        llclose(fd_trans, DISC);
    }
    siginterrupt(SIGALRM, TRUE);
}


void handle_alarm_timeout() {
    numTransmissions++;

    printf("Time out with %d transmissions.", numTransmissions);

    // When number of trans is exceeded
    if (numTransmissions > MAX_TRANS)
    {
        printf("Exceeded number of allowed transmissions.\n");
        closeDescriptor(fd_tr, &oldtio_trans);
        exit(-1);
    }
}


void alarm_off() {
    numTransmissions = 0;
    alarm(0);
}

int frame_i(char *data, char *frame, int data_len, char CMD){
    int fr_len, bcc_len= 1;

    // Stuffing bcc2 and data.
    char *BCC2 = (char*)malloc(sizeof(char));
    BCC2[0] = 0x00;

    for (int i = 0 ; i < data_len; i++){
        *BCC2 ^= data[i];
    }

    byte_stuffing(data, &data_len);
    byte_stuffing(BCC2, &bcc_len);

    // Store information
    fr_len = 5  + data_len + bcc_len;
    frame[0] = FLAG;
    frame[1] = A;
    frame[2] = CMD;
    frame[3] = frame[1]^frame[2];
    // BCC
    memcpy(&frame[4], data, data_len);
    memcpy(&frame[4 + data_len], BCC2, bcc_len);

    frame[fr_len-1] = FLAG;

    free(BCC2);

    return fr_len;
}
int byte_stuffing(char * frame, int* fr_len)
{
    char * tmp_frame;
    int ext_len = 0;        /* The extra space needed to be added to the frame. */
    int tmp_frame_len;   /* The new length of the string frame (extra + length). */
    int pos;         /* Position in the new_frame. */
    int counter = 0;            /* Number of escapes and flags found in the second iteration. */

    //  First find all the flags and scapes to avoid multiple reallocs.
    for (int i = 0 ; i < *fr_len; i++)
        if (frame[i] == FLAG || frame[i] == ESC) ext_len++;


    tmp_frame_len = ext_len + *fr_len;
    tmp_frame = (char *)malloc(sizeof(char) * tmp_frame_len);

    // insert an ESC before de FLAG and XOR with 0x20
    for (int i = 0 ; i< *fr_len; i++){
        pos = i + counter;
        if (frame[i] == FLAG){
            tmp_frame[pos] =  ESC;
            tmp_frame[pos+1] = FLAG ^ 0x20;

            counter ++;
        }
        //insert an ESC before de ESC and XOR with 0x20
        else if (frame[i] == ESC){
            tmp_frame[pos] = ESC;
            tmp_frame[pos+1] = ESC ^ 0x20;

            counter ++;
        }
        // otherwise is normally copied
        else tmp_frame[pos] = frame[i];
    }


    //frame = realloc(frame, new_frame_length);
    * fr_len  = tmp_frame_len;

    memcpy(frame, tmp_frame, tmp_frame_len);
    free(tmp_frame);
    return 0;
}
