// Application layer protocol implementation

#include "link_layer.c"

#include "application_layer.h"

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    install_alarm();
    LinkLayer conParameters;
    conParameters.timeout=timeout;
    conParameters.nRetransmissions=nTries;
    conParameters.baudRate=baudRate;
    conParameters.role=role;
    stcpy(conParameters.serialPort, serialPort);

    llopen(conParameters);
}
