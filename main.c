// Main file of the serial port project.
// NOTE: This file must not be changed.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/application_layer.h"
#include "include/variables.h"


// Arguments:
//   $1: /dev/ttySxx -> porta
//   $2: tx | rx    -> transmiter/ reciver
//   $3: filename
int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: %s /dev/ttySxx tx|rx filename\n", argv[0]);
        exit(1);
    }

    //verificar que a porta de serie é escrita correctamente
    if((strncmp(argv[1], "/dev/ttyS", 9)) != 0){
        //diferente
        printf("Incorrect serial port. The correct one is ... /dev/ttySxx");
        exit(-1);
    }

    const char *serialPort = argv[1];
    const char *role = argv[2];
    const char *filename = argv[3];
    /*int bd_in;

    while(TRUE){
      printf("Choose a Baudrate: \n"
            "1 - B1200\n"
            "2 - B2400\n"
            "3 - B4800\n"
            "4 - B19200\n"
            "5 - B38400\n"
            "6 - B115200\n"
            "Value: ");

      scanf("%d", &bd_in);

      if(bd_in> 6 || bd_in < 1){
        printf("Choose a valid baudrate!\n");
        continue;
      } else break;
    }
   

  int bd_a[] = {1200,2400, 4800, 19200, 38400, 115200};
  */

  printf("Starting link-layer protocol application\n"
        "  - Serial port: %s\n"
        "  - Role: %s\n"
        "  - Baudrate: %d\n"
        "  - Number of tries: %d\n"
        "  - Timeout: %d\n"
        "  - Filename: %s\n",
        serialPort,
        role,
        BAUDRATE,
        N_TRIES,
        TIMEOUT,
        filename);

    applicationLayer(serialPort, role, BAUDRATE, N_TRIES, TIMEOUT, filename);

    return 0;
}
