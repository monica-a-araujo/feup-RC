// Application layer protocol implementation

#include "link_layer.c"

#include "application_layer.h"

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


        llclose(TRUE, fd_trans, role);

    //receptor
    } else if (role == "rx"){
        int fd_rec;
        int p_rec;
        
        p_rec = openFile(filename, "wb");
        
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
int openFile(char filename, char opt){
    int ptr = fopen(filename, opt);
    if(ptr == NULL){
        printf("%s does not exist", filename);
        exit(-1);
    }
    return ptr;
}
