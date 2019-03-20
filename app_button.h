#ifndef __BSP_BUTTON_H
#define __BSP_BUTTON_H

#include "bsp_defines.h"

extern uint8_t buttonSTARTpressed;
extern uint8_t buttonRESETpressed;

int buttonSTART(int argc, char **argv);

#endif

