//
// Created by inessilva_132 on 13/10/22.
//

#ifndef FEUP_RC_VARIABLES_H
#define FEUP_RC_VARIABLES_H

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE       1


#define MAX_SIZE_ALLOC      16000           /* Max size for the vector. always must be higher than (FRAME_SIZE-10)*2 */
#define FRAME_SIZE          400             /*This size must not be less than 10*/

/* DEBUG MACROS ------------------------------------------------------------------------------------------------*/

#define DEBUG  1            /* Set zero to don't show prints. */
#define SHOW_OUTPUT 0      /* Show the information inside the state machines. */

/*
#define PRINT_SUC(format, ...) \
    do{ \
    if (DEBUG) \
    printf("\033[32;1mSUC\033[0m: %s:%d\t\t::\033[32;1m" format "\033[0m\n", __FILE__, __LINE__, ##__VA_ARGS__ ); \
    }while(0)

#define PRINT_NOTE(format, ...) \
    do{ \
    if (DEBUG) \
    printf("\e[1;34mNOTE\e[0m: %s:%d\t\t::\e[1;34m" format "\e[0m\n", __FILE__, __LINE__, ##__VA_ARGS__ ); \
    }while(0)

#define PRINTF(format, ...) \
    do{ \
    if (SHOW_OUTPUT) \
    printf("\e[1;34m"format "\e[0m", ##__VA_ARGS__ ); \
    }while(0)
*/

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
#define RR(r)           (0x05 | ( r << 7 ))
#define REJ(r)          (0x01 | ( r << 7 ))

/* MACROS FOR THE APPLICATION --------------------------------------------------------------------------------------*/

#define CTRL_DATA           1
#define CTRL_START          2
#define CTRL_END            3

#define T_FILE_SIZE         0
#define T_FILE_NAME         1

/* OTHERS  ---------------------------------------------------------------------------------------------------------*/
#define FALSE               0
#define TRUE                1
#define SWITCH(s)           !s
#define BIT(n)              (1 << n)
#define DELAY_US            0.2

/* OTHERS  ---------------------------------------------------------------------------------------------------------*/
#define BAUDRATE 9600 //medida de velocidade de sinaliza????o e representa o n??mero de mudan??as na linha de transmiss??o ou eventos por segundo
#define N_TRIES 3
#define TIMEOUT 4  

#endif //FEUP_RC_VARIABLES_H
