// Application layer protocol header.
// NOTE: This file must not be changed.

#ifndef _APPLICATION_LAYER_H_
#define _APPLICATION_LAYER_H_

// Application layer main function.
// Arguments:
//   serialPort: Serial port name (e.g., /dev/ttyS0).
//   role: Application role {"tx", "rx"}.
//   baudrate: Baudrate of the serial port.
//   nTries: Maximum number of frame retries.
//   timeout: Frame timeout.
//   filename: Name of the file to send / receive.


void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename);

//funções relacionadas com o ficheiro
void openFile(char* filename, char* opt);
int fsize(FILE *fp);

//funções relacionadas com o Control Package
int generate_controlPackage(char* fileName, int fileSize, char* pack,char s_or_e);

//funções relacionadas com o Data Package
int generate_dataPackage(int num_of_seq, char *info, int info_len, char *frame)


#endif // _APPLICATION_LAYER_H_
