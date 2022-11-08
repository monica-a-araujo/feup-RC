// Link layer protocol implementation

#include "../include/link_layer.h"
#include "../include/variables.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

int numTransmissions = 0;
int fd_trans = 0;
const struct termios oldtio_trans;
const struct termios oldtio_rec;


// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source


/*
int closefd(int fd, struct termios * oldtio) {

    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
        perror("tcsetattr");ghjkg
        exit(-1);
    }
    printf("Success : Oldtio restoured");
    return close(fd);
}
*/

int llopen(LinkLayer connectionParameters)
{
    printf("llopen\n");
    int fd;
    int connection = -1; //connection inicialmente nao esta estabelecida
    int sent = -1;


    //erro-tipo de conexão errada
    if (connectionParameters.role != tx && connectionParameters.role != rx){
        printf("Actual flag %d. Must be %d or %d", connectionParameters.role, tx, rx);
        return -1;
    }

    //Conexão com o emissor
    if(connectionParameters.role == tx){
        printf("llopen trans\n");
        //abrir file descriptor
        fd = openfd(connectionParameters.serialPort, connectionParameters.baudRate);


        while (connection < 0 && sent < 0){
            //iniciar alarme
            alarm(connectionParameters.timeout);

            //mandar set (verificar que se mandou mesmo e se não há erro)
            if((sent = sendframe_S_U(fd, A, SET))<0){
                printf("Send frame SET fail. Sending again after timeout.\n");
            } else printf("Send SET with success.\n");

            //ler UA (verificar que se mandou o certo, se não erro)
            if((connection = readframe_NS_A(fd, UA)) < 0){
                printf("Still not received UA. Starting again.\n");
            } else printf("Receive UA.\n");
            printf("1 - %d - %d\n", sent, connection);
       }

       if(connection == 0 && sent >= 0){ //o frame set e UA foram enviados e recebidos corretamente e a conexão foi estabelecida.
        turnOffAlarm();
       }
    }

    //conexão com o receptor
    else if(connectionParameters.role == rx){
        //abrir file descriptor
        fd = openfd(connectionParameters.serialPort, connectionParameters.baudRate);

        while (connection < 0 && sent < 0){
            //ler set e verificações padrão
            if((connection = readframe_NS_A(fd, SET)) < 0){
                printf("Still not received SET.\n");
            } else printf("Receive SET.\n");

            //mandar UA e verificações padrão
            if((sent = sendframe_S_U(fd, A, UA))<0){
                printf("Send frame UA fail\n");
            } else printf("Send UA with success.\n");
            printf("dois %d ponto %d\n", sent, connection);
        }
    }
    return fd;
}

int openfd(char serialPort[50],int baudRate){
    printf("openfd enter\n");
    struct termios *oldtio;
    struct termios *newtio;

    // Open serial port device for writing and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    int fd = open(serialPort, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        printf("error, openfd serial port\n");
        perror(serialPort);
        exit(-1);
    }


    // Save current port settings
    if (tcgetattr(fd, oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(newtio, 0, sizeof(newtio));

    // BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
    // CRTSCTS : output hardware flow control
    //           ATTENTION:  only used if the cable has all necessary lines (is
    //           a null modem cable), otherwise the output flow control is set
    //           automatically by software by the driver.
    // CS8     : 8n1 (8bit,no parity,1 stopbit)
    // CLOCAL  : local connection, no modem control
    // CREAD   : enable receiving characters
    newtio->c_cflag = baudRate | CS8 | CLOCAL | CREAD;

    // IGNPAR: Ignore framing and parity errors
    // ICRNL:  Map CR to NL (otherwise a CR input on the other computer
    //         will not terminate input)
    // Otherwise make device raw (no other input processing)
    newtio->c_iflag = IGNPAR | ICRNL;

    // Raw output
    newtio->c_oflag = 0;

    // ICANON : enable canonical input
    // disable all echo functionality, and don't send signals to calling program
    newtio->c_lflag = ICANON;

    // Initialize all control characters
    // default values can be found in /usr/include/termios.h, and are given
    // in the comments, but we don't need them here
    newtio->c_cc[VINTR] = 0;    // Ctrl-c
    newtio->c_cc[VQUIT] = 0;    // Ctrl-'\'
    newtio->c_cc[VERASE] = 0;   // del
    newtio->c_cc[VKILL] = 0;    // @
    newtio->c_cc[VEOF] = 4;     // Ctrl-d
    newtio->c_cc[VTIME] = 0;    // inter-character timer unused
    newtio->c_cc[VMIN] = 1;     // blocking read until 1 character arrives
    newtio->c_cc[VSWTC] = 0;    // '\0'
    newtio->c_cc[VSTART] = 0;   // Ctrl-q
    newtio->c_cc[VSTOP] = 0;    // Ctrl-s
    newtio->c_cc[VSUSP] = 0;    // Ctrl-z
    newtio->c_cc[VEOL] = 0;     // '\0'
    newtio->c_cc[VREPRINT] = 0; // Ctrl-r
    newtio->c_cc[VDISCARD] = 0; // Ctrl-u
    newtio->c_cc[VWERASE] = 0;  // Ctrl-w
    newtio->c_cc[VLNEXT] = 0;   // Ctrl-v
    newtio->c_cc[VEOL2] = 0;    // '\0'

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred  to
    // by  fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, newtio) == -1)
    {
        perror("tcsetattr here");
        exit(-1);
    }

    printf("New termios structure set\n");

    return fd;
}
//sucesso return > 0 (numero de bytes escritos)
//erro return == -1
int sendframe_S_U(int fd, char addressField, char controlField){
    char frame[5];
    frame[0]= FLAG;
    frame[1]= addressField;
    frame[2]= controlField;
    frame[3]= frame[1] ^ frame[2];
    frame[4]= FLAG;
    printf("send frame s_u: %d, %d, %d, %d, %d", FLAG, addressField, controlField,frame[1] ^ frame[2],FLAG );

    return write(fd, frame, 5);

    

}
//ler UA
//return -1: erro   0:sucesso
int readframe_NS_A(int fd, char controlField){
    int state=0; //state da maquina de estados, inicialmente 0
    char buffer;
    int analy;

    while(TRUE){
        //ler o campo do outro terminal, se não conseguirmos, dá erro

        //NOTA: não consegui confirmar, mas pelo que entendi o read escreve no frame - buffer
        //      e depois sempre que é chamado outra vez, volta a rescrever por cima. - CERTO
        if((analy = read(fd, &buffer, 1))==1){
            printf("read frame receve something\n");
            continue;
        } else if (analy == -1){
            printf("read frame receve nothing\n");
            return -1;
        }

        switch (state)
        {
        case 0:
            if(buffer==FLAG){
                state =1;
            }
            break;

        case 1:
            if (buffer == A){
                state = 2;
            } else changeState(buffer, &state);
            break;

        case 2:
            if(buffer == controlField){
                state=3;
            } else changeState(buffer, &state);
            break;

        case 3:
            if (buffer == (controlField ^ A)){
                state = 4;
            } else changeState(buffer, &state);
            break;

        case 4:
            if (buffer == FLAG){
                return 0; //concluido, programa leu a frame pretendida
            } else state =0;
            break;
        }
    }

}

//ler RR e REJ
int readframe_S_A(int fd, char *controlField){
    int state = 0; //state da maquina de estados, inicialmente 0
    char buffer;
    int analy;

    while(TRUE){
        //ler o campo do outro terminal, se não conseguirmos, dá erro

        //NOTA: não consegui confirmar, mas pelo que entendi o read escreve no frame - buffer
        //      e depois sempre que é chamado outra vez, volta a rescrever por cima. - CERTO
        if((analy = read(fd, &buffer, 1))==1){
            continue;
        } else if (analy == -1){
            return -1;
        }

        switch (state)
        {
        case 0:
            if(buffer==FLAG){
                state=1;
            }
            break;

        case 1:
            if (buffer == A){
                state = 2;
            } else changeState(buffer, &state);
            break;

        case 2:
            //Nota: o que significa o rr/rej começar com 0 ou 1 (binario)
            if(buffer == RR(0) || buffer == RR(1) || buffer == REJ(0) || buffer == REJ(1)){
                state = 3;
                *controlField = buffer;
            } else changeState(buffer, &state);
            break;

        case 3:
            if (buffer== (atoi(controlField) ^ A)){
                state =4;
            } else changeState(buffer, &state);
            break;

        case 4:
            if (buffer == FLAG){
                return 0; //concluido, programa leu a frame pretendida
            } else state =0;
            break;
        }
    }

}

void changeState(char buffer, int *state){
    if (buffer == FLAG){
        *state = 1;
    } else *state = 0;
}

int llwrite(int fd, char *buf, int *bufSize){
    static int sval_sen = 0;  // 0S000000 s = N(s) -> número de sequência
    int fr_len;
    char controlField;

    char * fr = (char*) malloc(MAX_SIZE_ALLOC*sizeof(char));
    char * buf_cpy = (char*) malloc(MAX_SIZE_ALLOC*sizeof(char));

    if(bufSize < 0){
        printf("Invalid length : %d\n",*bufSize);
        return -1;
    }

    memcpy(buf_cpy, buf, *bufSize);

    for(int i = 0;TRUE;i++){

        memcpy(buf,buf_cpy, *bufSize);
        memset(fr,0,strlen(fr));

        fr_len = frame_i_generator(buf, fr, bufSize, (0x00 | (sval_sen << 6 )));
        alarm(TIMEOUT);
        if( write(fd,fr,fr_len) < 0){
            printf("Sender wasn't able to write into frame.");
            continue;
        }
        else printf("Frame sent with S = %d",sval_sen);

        if(readframe_S_A(fd, &controlField) < 0){
            printf("Wasn't able to read info frame.");
            continue;
        }else printf("Succeded to read controlField= %02x with R =%d ", controlField, !sval_sen);

        if (((controlField == (0x05 | ( 1 << 7 ))) && (sval_sen == 0)) || (controlField == (0x05 | ( 0 << 7 )) && (sval_sen == 1))){
            turnOffAlarm();
            sval_sen = !sval_sen;
            free(fr);
            free(buf_cpy);
            return 0;
        }
        if (controlField == (0x01 | ( !sval_sen << 7 )) || controlField == (0x01 | ( sval_sen << 7 ))){
            turnOffAlarm();
            printf("Error : Received REJ SIGNAL");
            continue;
        }
    }

}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(char *packet, int fd){
    int packet_len = -1;

    static int s_r = 0, cur_s = 0;
    char CMD, BCC2_CHECK;

    for(int i=0;TRUE;i++) {
        if ((packet_len = read_frame_i(fd, packet, &CMD)) >= 0) {
            printf("S_CMD = %02x was received with S = %d",(0x00 | ( s_r << 6 )),s_r);
        }
        else{
            sleep(0.2);
            printf("Trying to re-read.");
            continue;

    }
        if (CMD == (0x00 | ( 0 << 6 ))) cur_s = 0;
        else if (CMD == (0x00 | ( 1 << 6 ))) cur_s = 1;

        destuffing(packet,&packet_len);

        BCC2_CHECK = 0x00;
        for (int i = 0 ; i < packet_len-1; i++){
            BCC2_CHECK ^= packet[i];
        }

        if(BCC2_CHECK != packet[packet_len-1]) {
            printf("The expected BCC2 was : %02x, which is different from the one received : %02x",BCC2_CHECK
                   ,packet[packet_len-1]);
            if (CMD == (0x00 | ( 0 << 6 )))
                sendframe_S_U(fd, 0x03 , 0x01 | ( 1 << 7 ));
            else if (CMD == (0x00 | ( 1 << 6 )))
                sendframe_S_U(fd, 0x03 , 0x01 | ( 0 << 7 ));
            continue;

        }else{
            printf("Got expected BCC2: %02x",packet[packet_len-1]);
    }

        // It's not the desired message.
        if (CMD != (0x00 | ( s_r << 6 ))){
            printf("Undesired message %d, RR will be sent", s_r);

            sendframe_S_U(fd, 0x03,  (0x05 | ( !cur_s << 7 )));
            continue;       // Discard the message.
        }

        // Desired message, save the info.
        if (sendframe_S_U(fd, 0x03,  (0x05 | ( s_r << 7 ))) > 0){
            printf("CMD_RR with R=%d sent.", !s_r);
            s_r = !s_r;
            return packet_len;
        }

    }
    return -1;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int fd, const char * role)
{
    int desconnect = -1; //connection inicialmente ainda esta estabelecida


    //erro - tipo de conecção errada
    if (strcmp(role, "tx") != 0  && strcmp(role, "rx") != 0 ){
        printf("Actual flag %s. Must be %d or %d", role, tx, rx);
        return -1;
    }

    //Conneçao com o emissor
    if(strcmp(role, "tx") == 0 ){

        while (desconnect < 0){
            //iniciar alarme
            alarm(TIMEOUT);

            //mandar DISC (verificar que se mandou mesmo e se não há erro)
            if((desconnect = sendframe_S_U(fd, A, DISC))<0){
                printf("Send frame DISC fail. Sending again after timeout.\n");
                continue;
            } else printf("Send DISC with success.\n");

            //ler DISC (verificar que se mandou o certo, se não erro)
            if((desconnect = readframe_NS_A(fd, DISC)) < 0){
                printf("Still not receve DISC. Starting again.");
                continue;
            } else printf("Receive DISC.");

            //mandar UA (verificar que se mandou mesmo e se não há erro)
            if(sendframe_S_U(fd, A, UA)<0) {
                printf("Send frame UA fail. Sending again after timeout.\n");
                continue;
            } else printf("Send DISC with success.\n");
       }
       
       if (tcsetattr(fd, TCSANOW, &oldtio_trans) == -1) {
            perror("tcsetattr");
            exit(-1);
        }
        printf("Success : Oldtio restoured");
        return close(fd);

       //return closefd(fd, &oldtio_trans);
    }
     //conexão com o recetor
    else if(strcmp(role, "rx") == 0 ){

        while (desconnect < 0){

            //ler DISC e verificações padrão
            if((desconnect = readframe_NS_A(fd, DISC)) < 0){
                printf("Still not receve DISC.\n");
                continue;
            } else printf("Receive DISC.\n");

            //mandar DISC e verificações padrão
            if((desconnect = sendframe_S_U(fd, A, DISC))<0){
                printf("Send frame DISC fail\n");
                continue;
            } else printf("Send DISC with success.\n");

            //ler UA e verificações padrão
            if((desconnect = readframe_NS_A(fd, UA)) < 0){
                printf("Still not receve DISC.\n");
                continue;
            } else printf("Receive DISC.\n");

        }
        if (tcsetattr(fd, TCSANOW, &oldtio_rec) == -1) {
            perror("tcsetattr");
            exit(-1);
        }
        printf("Success : Oldtio restoured");
        return close(fd);

        //return closefd(fd, &oldtio_rec);
    }



    return -1; //se der erro
}
int read_frame_i(int fd, char *buf, char *CMD){
    int cur_state = 0;
    int i_len = -1;
    char byte;

    while(cur_state < 5){
        if (read(fd, &byte, 1) == -1)
            return -1;

        switch (cur_state)
        {
            // RECEBE A FLAG, SE COINCIDIR PASSA PARA O STATE 1
            case 0:
                i_len = 0;
                printf("Analyzing the flag - case 0: %02x\n", byte);
                if (FLAG == byte)
                    cur_state ++;
                break;
                // RECEBE O ADDR, SE FOR O ADDR AVANÇA PARA O STATE 2, SENÃO SE AINDA N FOR A FLAG REGRESSA AO ZERO ATÉ RECEBER
            case 1:
                printf("Analyzing the ADDR - case 1: %02x\n", byte);
                if (A == byte)
                    cur_state ++;
                else if (FLAG != byte)
                    cur_state = 0;
                break;

                // RECEBE O CMD
            case 2:
                printf("Analyzing CMD - case 2: %02x\n", byte);
                if (byte == CMD_S(0) || byte == CMD_S(1)){
                    *CMD = byte;
                    cur_state++;
                }
                // SE RECEBEMOS A FLAG VOLTAMOS AO ESTADO 1, OU SEJA ESPERAMOS NOVAMENTE PELO ADDR
                else if (byte == FLAG)
                    cur_state = 1;
                else cur_state = 0;

                break;

                // RECEBE O BCC1
            case 3:
                printf("Analyzing BCC1 - case 3: %02x\n", byte);
                if (byte == (*CMD ^ A))
                    cur_state ++;
                // SE RECEBEMOS A FLAG VOLTAMOS AO ESTADO 1, OU SEJA ESPERAMOS NOVAMENTE PELO ADDR
                else if (byte == FLAG)
                    cur_state = 1;
                else
                    cur_state = 0;
                break;
                // RECEBE A INFORMAÇÃO E PREENCHEMOS O BUF
            case 4:
                printf("Getting info - case 4: %02x\n", byte);
                if (byte != FLAG){
                    buf[i_len++] = byte;
                }//SENÃO AVANÇA INVALIDANDO O ESTADO QUEBRANDO O CICLO
                else cur_state ++;

        }
    }
    return i_len;
}

void install_alarm() {
    if (signal(SIGALRM, handle_alarm_timeout) == SIG_ERR)
    {
        printf("It wasn't possible to install signal.");
        exit(-1);
    }
    printf("install_alarm with success\n");
    siginterrupt(SIGALRM, TRUE);
}


void handle_alarm_timeout() {
    numTransmissions++;

    printf("Time out with %d transmissions.", numTransmissions);

    // When number of trans is exceeded
    if (numTransmissions > MAX_TRANS)
    {
        printf("Exceeded number of allowed transmissions.\n");

        if (tcsetattr(fd_trans, TCSANOW, &oldtio_trans) == -1) {
            perror("tcsetattr");
            exit(-1);
        }
        printf("Success : Oldtio restoured");
        close(fd_trans);

        //closefd(fd_trans, &oldtio_trans);

        exit(-1);
    }
}


void turnOffAlarm() {
    numTransmissions = 0;
    alarm(0);
}


int frame_i_generator(char *data, char *frame, int data_len, char controlField){
    int fr_len, bcc_len= 1;

    // Stuffing bcc2 and data.
    char *BCC2 = (char*)malloc(sizeof(char));
    BCC2[0] = 0x00;

    for (int i = 0 ; i < data_len; i++){
        *BCC2 ^= data[i];
    }

    stuffing(data, &data_len);
    stuffing(BCC2, &bcc_len);

    // Store information
    fr_len = 5  + data_len + bcc_len;
    frame[0] = FLAG;
    frame[1] = A;
    frame[2] = controlField;
    frame[3] = frame[1]^frame[2]; // BCC1

    memcpy(&frame[4], data, data_len);
    memcpy(&frame[4 + data_len], BCC2, bcc_len);

    frame[fr_len-1] = FLAG;

    free(BCC2);

    return fr_len;
}
int stuffing(char * frame, int* fr_len){
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

int destuffing(char * packet, int* packet_len){

int tmp_packet_pos = 0;
char * tmp_packet = (char*)malloc(sizeof(char)*(*packet_len));

for (int i = 0 ; i < *packet_len; i++){
    if (packet[i] == ESC){
        if (packet[i+1] ==( FLAG ^ 0x20))
            tmp_packet[tmp_packet_pos] = FLAG;
        else if (packet[i+1] == (ESC ^ 0x20))
            tmp_packet[tmp_packet_pos] = ESC;

        i++;
    }
    else tmp_packet[tmp_packet_pos] = packet[i];
    tmp_packet_pos ++;
}


memcpy(packet, tmp_packet, tmp_packet_pos);
*packet_len = tmp_packet_pos;
free(tmp_packet);
return 0;

}

