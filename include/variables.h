//
// Created by inessilva_132 on 13/10/22.
//



#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE       1


#define MAX_SIZE_ALLOC      16000           /* Max size for the vector. always must be higher than (FRAME_SIZE-10)*2 */
#define FRAME_SIZE          400             /*This size must not be less than 10*/


/* MACROS FOR THE LLOPEN FLAG --------------------------------------------------------------------------------------*/

#define TRANSMITTER         0
#define RECEPTOR            1

/* MACROS FOR THE ALARM AND LOOPS---------------------------------------------------------------------------------- */

#define MAX_TRANS           3   /* Tries to read the receptor answers*/
#define TRIES_READ          3   
/* MACROS FOR THE PROTOCOL-------------------------------------------------------------------------------------------*/

#define FLAG                0x7E
#define ESC                 0x7D
#define A                   0x03

/**Command Field*/

#define SET             0x03        /* SET command*/
#define DISC            0x0B        /* DISC command*/
#define UA              0x07        /* UA command*/
#define CMD_S(s)        (0x00 | ( s << 6 ))
#define RR(r)           (0x05 | ( r << 7 ))
#define REJ(r)          (0x01 | ( r << 7 ))

/* MACROS FOR THE APPLICATION --------------------------------------------------------------------------------------*/

#define DATA           1
#define START          2
#define END            3

#define TYPE_FILE_SIZE          0
#define TYPE_FILE_NAME          1

/* OTHERS  ---------------------------------------------------------------------------------------------------------*/
#define FALSE               0
#define TRUE                1
#define SWITCH(s)           !s
#define BIT(n)              (1 << n)

/* OTHERS  ---------------------------------------------------------------------------------------------------------*/
#define BAUDRATE 38400 //medida de velocidade de sinalização e representa o número de mudanças na linha de transmissão ou eventos por segundo
#define N_TRIES 3
#define TIMEOUT 4  


