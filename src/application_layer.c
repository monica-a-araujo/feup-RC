// Application layer protocol implementation

#include "link_layer.c"

#include "application_layer.h"

int seqNumber_rx = 0;

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{ 
    LinkLayer conParameters;
    conParameters.timeout=timeout;
    conParameters.nRetransmissions=nTries;
    conParameters.baudRate=baudRate;
    conParameters.role=role;
    strcpy(conParameters.serialPort, serialPort);

    // transmiter
    if(role== "tx"){
        int fd_trans; //file descriptor
        int p_trans;
        
        p_trans = openFile(filename, "rb");
        install_alarm();
        fd_trans=llopen(conParameters);
        
        //control packege
        //data packege
        //control package


        llclose(fd_trans, role);

    //receptor
    } else if (role == "rx"){

        int fd_rec;
        int p_rec;
        int read_start = FALSE;
        char *packet;
        int filesize;
        int size_packet;
        packet = (char *) malloc(max_size()*sizeof(char));
        char *data;
        data = (char *) malloc(max_size()*sizeof(char));
        char *out_filename;
        out_filename = (char*) malloc(max_size()*sizeof(char));
        int out_filesize;
        
        
        //open channel
        fd_rec = llopen(conParameters);

        //recceive control packet (start)
        while(read_start == TRUE){
            size_packet =  llread(packet, fd_rec);
            if(packet[0] = Control_start){
                printf("start packet in receiver");
                Read_controlPacket(packet, filename, &filesize, size_packet);
                read_start = FALSE;
            }
        }
        // opne file with data in the control packet
        p_rec = openFile(filename, "wb");

        //read data
        while(TRUE){
            size_packet =  llread(packet, fd_rec);

            if(packet[0]== control_data){
                size_packet = Read_dataPacket(&seqNumber_rx, data, packet);
                printf("Sequence Number: %d\n", seqNumber_rx);
                fwrite(data, sizeof(char), size_packet, fd_rec);
            }

            //limpar string data
            memset(data, 0, strlen(data));

            if(packet[0]== control_end){
                
                Read_controlPacket(packet, out_filename, out_filesize, size_packet);
                if(strcmp(out_filename, filename) != 0 && strcmp(out_filesize, filesize)){
                    printf("End file name: %s\tBegin file name: %s\n", out_filename, filename);
                    printf("End file size: %s\tBegin file size: %s\n", out_filesize, filesize);
                }
                //ja encontramos o end control packet por isso podemos sair do ciclo
                break;
            }
        }

        printf("File name: %s\t File size: %s\n", filename, filesize);
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
int Read_controlPacket(char *packet, char *filename, int *filesize, int size_packet){
    char *filesize_in_char;

        for (int i = 0; i < size_packet; i++){

            //type of information: file size
            if(type_file_size == packet[i]){
                i++;
                int length_file_size = packet[i];
                filesize_in_char = (char*) malloc(length_file_size);
                i++; 
                if(memcpy(filesize_in_char, &packet[i], length_file_size) == NULL){
                    printf("Doesn't found the file size");
                    return -1;
                }
                filesize = atoi(filesize_in_char); //passar de char* para int*
                i+=length_file_size-1;
                free(filesize_in_char);

            //type of information: file name
            } else if(type_file_name == packet[i]){
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
    seqNumber_rx = packet[1];
    int datasize = 256 * packet[2] + packet[3];

    if(memcpy(data, &packet[4], datasize) == NULL){
        printf("Doesn't found the data from file");
        return -1;
    }

    return datasize;
}

int openFile(char filename, char opt){
    int ptr = fopen(filename, opt);
    if(ptr == NULL){
        printf("%s does not exist", filename);
        exit(-1);
    }
    return ptr;
}
