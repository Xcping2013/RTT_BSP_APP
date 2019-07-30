#ifndef __APP_SERIAL_H
#define __APP_SERIAL_H

#include "bsp_defines.h"

int uart_stream_thread_init(void);
int printdata(int argc, char **argv);
int printdata_disable(int argc, char **argv);

void openSerial(void);
void closeSerial(void);

#endif
