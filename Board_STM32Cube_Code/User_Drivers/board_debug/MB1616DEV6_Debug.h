#ifndef __BOARD_DEBUG_H
#define __BOARD_DEBUG_H

#include "sys.h"

extern uint8_t auto_test_mode;
	
void board_debug(void);

void motor_cyliner_test(void);

void add_test_mode(uint8_t type, uint8_t enable);

#endif

