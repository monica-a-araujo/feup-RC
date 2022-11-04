// Application layer protocol implementation

//#include "link_layer.c"
#include "../include/link_layer.h"
#include "../include/variables.h"
#include "../include/application_layer.h"
#include <string.h>
#include <stdlib.h>


int seqNumber_rx = 0;

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename) {

    LinkLayer conParameters;
    conParameters.timeout = timeout;
    conParameters.nRetransmissions = nTries;
    conParameters.baudRate = baudRate;

    if(strcmp(role, "tx") == 0) conParameters.role = tx;
    else if(strcmp(role, "rx") == 0) conParameters.role = rx;

    strcpy(conParameters.serialPort, serialPort);
    

    FILE* f_trans;
    int seq_numbertx = 0;                    //número de sequência,
                                             // por cada byte enviado
                                             // é incrementado


    // transmissor
    if (strcmp(role, "tx") == 0) {

        int fd_trans; //file descriptor

        int dataSize = 390;
        int frame_len = 0;
        int actual_Size = 0;

        char * content[dataSize]; // conteúdo do ficheiro
        char* fr = (char*)malloc(sizeof(char)*MAX_SIZE_ALLOC);  //partes do ficheiro
        char* packet[MAX_SIZE_ALLOC];  

        f_trans = openFile(filename, "rb");
        int fileSize = fsize(f_trans);

        install_alarm();   // ativar o alarme

        fd_trans = llopen(conParameters);  // envio de uma trama SET e receção de uma trama UA

        //control package START

        int C_size = generate_controlPackage(filename, fileSize, packet ,START);

        llwrite(fd_trans, packet, &C_size);
        //gerar o data Package

        for(int j = 0 ;TRUE;j++){
            if (fileSize - seq_numbertx * dataSize < dataSize ) dataSize = fileSize%dataSize;
            if ((actual_Size = fread(content, 1, dataSize, f_trans)) <= 0) {
                break;
            }

            if (generate_dataPackage(seq_numbertx, content, actual_Size, fr) <0){
               printf("Error : Data package creation wasn't succesful\n");
                exit(-1);
            }

            frame_len = actual_Size + 4;
            if (llwrite(fd_trans, fr, &frame_len) < 0) {
                printf("Error in Llwrite\n");
                exit(-1);
            }

            seq_numbertx++;
        }


        // control package END
        C_size = generate_controlPackage(filename, fileSize, packet,END);
        llwrite(fd_trans, packet, &C_size);

        free(fr);

        //CLOSE
        llclose(fd_trans, role);
    }

    //receptor
    else if (strcmp(role, "rx") == 0){

        FILE *f_rec;

        int fd_rec;
        int read_start = FALSE;
        char *packet;
        int filesize;
        int size_packet;
        packet = (char *) malloc(MAX_SIZE_ALLOC*sizeof(char));
        char *data;
        data = (char *) malloc(MAX_SIZE_ALLOC*sizeof(char));
        char *out_filename;
        out_filename = (char*) malloc(MAX_SIZE_ALLOC*sizeof(char));
        int out_filesize;
        
        
        //open channel
        fd_rec = llopen(conParameters);

        //recceive control packet (start)
        while(read_start == TRUE){
            size_packet =  llread(packet, fd_rec);
            if(packet[0] == START){
                printf("start packet in receiver\n");
                Read_controlPacket(packet, filename, &filesize, size_packet);
                read_start = FALSE;
            }
        }
        // opne file with data in the control packet
        f_rec = openFile(filename, "wb");

        //read data
        while(TRUE){
            size_packet =  llread(packet, fd_rec);

            if(packet[0]== DATA){
                size_packet = Read_dataPacket(&seqNumber_rx, data, packet);
                printf("Sequence Number: %d\n", seqNumber_rx);
                fwrite(data, sizeof(char), size_packet, f_rec);
            }

            //limpar string data
            memset(data, 0, strlen(data));

            if(packet[0]== END){
                
                Read_controlPacket(packet, out_filename, &out_filesize, size_packet);
                if(strcmp(out_filename, filename) != 0 && (out_filesize != filesize)){
                    printf("End file name: %s\tBegin file name: %s\n", out_filename, filename);
                    printf("End file size: %d\tBegin file size: %d\n", out_filesize, filesize);
                }
                //ja encontramos o end control packet por isso podemos sair do ciclo
                break;
            }
        }

        printf("File name: %s\t File size: %d\n", filename, filesize);
        llclose(fd_rec, role);
        free(packet);
        free(data);
        free(out_filename);

        //TODO: close file
    }
}
//Read the control packets
//discover the name and size of the file
//(Fill the filename field and filesize field)
int Read_controlPacket(char *packet,  const char *filename, int *filesize, int size_packet){
    char *filesize_in_char;

        for (int i = 0; i < size_packet; i++){

            //type of information: file size
            if(TYPE_FILE_SIZE == packet[i]){
                i++;
                int length_file_size = packet[i];
                filesize_in_char = (char*) malloc(length_file_size);
                i++; 
                if(memcpy(filesize_in_char, &packet[i], length_file_size) == NULL){
                    printf("Doesn't found the file size");
                    return -1;
                }
                *filesize = atoi(filesize_in_char); //passar de char* para int*
                i+=length_file_size-1;
                free(filesize_in_char);

            //type of information: file name
            } else if(TYPE_FILE_NAME == packet[i]){
                i++;
                int length_file_name = packet[i];
                i++;
                if(memcpy(filename, &packet[i], length_file_name) == NULL){
                    printf("doesn't found the file name");
                }
                i+=length_file_name-1;
            }
        }
        printf("Read successfully control packet");

        return 0;
        

}

//Read data packets
//fill the informaton of packet and give the size of packet
int Read_dataPacket(int *seqNumber_rx, char *data, char *packet){
    *seqNumber_rx = packet[1];
    int datasize = 256 * packet[2] + packet[3];

    if(memcpy(data, &packet[4], datasize) == NULL){
        printf("Doesn't found the data from file");
        return -1;
    }

    return datasize;
}

FILE* openFile(const char* filename, char* opt){
    FILE* f = fopen(filename, opt);
    if( f == NULL){
        printf("%s does not exist", filename);
        exit(-1);
    }
    printf("file open with success\n");
    return f;
}


int generate_controlPackage(const char* fileName, int fileSize, char** packet,char s_or_e){
    int curr_pos = 0;

    packet[0]= s_or_e;
    packet[1] = TYPE_FILE_SIZE;

    char * length_string = (char*)malloc(sizeof(int));
    sprintf(length_string, "%d", fileSize);
    packet[2] = strlen(length_string);

    if (memcpy(&packet[3], length_string, strlen(length_string)) == NULL){
        printf("Error :  Unable to copy file name");
        return -1;
    }
    curr_pos = 3 + strlen(length_string);

    packet[curr_pos] = TYPE_FILE_NAME;
    packet[curr_pos+1] = strlen(fileName);

    if (memcpy(&packet[curr_pos+2] , fileName, strlen(fileName)) == NULL){
        printf("Error : Unable to copy size of file");
        return -1;
    }

    printf("Success : Control package created.");
    return curr_pos + strlen(fileName) + 2;
}

int generate_dataPackage(int num_of_seq, char *info, int info_len, char *frame) {

    frame[0] = 1;
    frame[1] = num_of_seq % 256;
    frame[2] = info_len / 256; // L2
    frame[3] = info_len % 256; // L1

    if (memcpy(&frame[4], info, info_len) == NULL) {
        printf("Error : Package copy wasn't possible.");
        return -1;
    }
    printf("Success : Data package was generated.");
    return 0;
}

// função para determinar o tamanho do ficheiro
int fsize(FILE *fp){
    int prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); //go back to where we were
    printf("file size with success\n");
    return sz;
}