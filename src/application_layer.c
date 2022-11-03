// Application layer protocol implementation

#include "link_layer.c"

#include "application_layer.h"

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename) {

    LinkLayer conParameters;
    conParameters.timeout = timeout;
    conParameters.nRetransmissions = nTries;
    conParameters.baudRate = baudRate;
    conParameters.role = role;
    strcpy(conParameters.serialPort, serialPort);
    
    int main(int argc, char **argv){

        FILE* f;
        int seq_numbertx = 0                    //número de sequência,
                                                // por cada byte enviado
                                                // é incrementado


    // transmissor
    if (role == "tx") {
        int fd_trans; //file descriptor

        int dataSize = 390, frame_len = 0 , actual_Size = 0; // verificar pq

        char * content[contentSize]; // conteúdo do ficheiro
        char* fr = (char*)malloc(sizeof(char)*MAX_SIZE_ALLOC);  //partes do ficheiro
        char* packet[MAX_SIZE_ALLOC];  //
        // fazer a comparação com o path e o role tx;
        char* fileName = argv[2];

        openFile(fileName, "rb");
        int fileSize = fsize(f);

        install_alarm();   // ativar o alarme

        fd_trans = llopen(conParameters);  // envio de uma trama SET e receção de uma trama UA

        //control package START

        int C_size = generate_controlPackage(fileName, fileSize, packet ,START);

        llwrite(fd_trans, pack, &C_start_size);
        //gerar o data Package


        for(int j = 0 ;TRUE;j++){
            if (fileSize - seq_numbtx * dataSize < dataSize ) dataSize = fileSize%dataSize;
            if ((actual_Size = fread(content, 1, contentSize, f)) <= 0) {
                break;
            }

            if (generate_dataPackage(seq_numbtx, content, actual_Size, fr) <0){
               printf("Error : Data package creation wasn't succesful");
                return -1;
            }

            frame_len = actual_Size + 4;
            if (llwrite(fd, fr, &frame_len) < 0) {
                PRINT_ERR("Error in Llwrite");
                return -1;
            }

            seq_numbtx++;
        }


        // control package END
        C_size = generate_controlPackage(filename, fileSize, packet,END);
        llwrite(fd, packet, &size);

        free(fr);

        //CLOSE
        llclose(TRUE, fd_trans, role);
    }

        //receptor
    } else if (role == "rx") {
        int fd_rec;

        openFile(filename, "wb");

        fd_rec = llopen(conParameters);

        /*
        ...
        teste
        ...
        */
        int i = 1;


        //TODO: perceber que estatistica e que e para mostar
        llclose(TRUE, fd_rec, role);
    }
}




void openFile(char* filename, char* opt){
     f = fopen(filename, opt);
    if( f == NULL){
        printf("%s does not exist", filename);
        exit(-1);
    }
}


int generate_controlPackage(char* fileName, int fileSize, char* pack,char s_or_e){
    int size_nameFile = strlen(nameFile), curr_pos = 0;

    pack[0]= s_or_e;
    pack[1] = T_FILE_SIZE;

    char * length_string = (char*)malloc(sizeof(int));
    sprintf(length_string, "%d", fileSize);
    pack[2] = strlen(length_string);

    if (memcpy(&pack[3], length_string, strlen(length_string)) == NULL){
        printf("Error :  Unable to copy file name");
        return -1;
    }
    curr_pos = 3 + strlen(length_string);

    pack[curr_pos] = T_FILE_NAME;
    pack[curr_pos+1] = strlen(fileName);

    if (memcpy(&pack[curr_pos+2] , fileName, strlen(fileName)) == NULL){
        printf("Error : Unable to copy size of file");
        return -1;
    }

    printf("Success : Control package created.");
    return curr_pos + strlen(fileName) + 2;
}

int generate_dataPackage(int num_of_seq, char *info, int info_len, char *frame) {

    frame[0] = 1;
    frame[1] = num_of_seq % 256;
    frame[2] = len / 256; // L2
    frame[3] = len % 256; // L1

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
        return sz;
    }