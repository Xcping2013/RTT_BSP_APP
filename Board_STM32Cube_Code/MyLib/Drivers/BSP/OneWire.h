#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#include "stm32f1xx_hal.h"

typedef struct
{		
  u8 		(*Init)(void );
//	void 	(*Reset)(void );
//	void 	(*StartConv)(void );
	short (*GetData)(void);																		
	
}OneWire_FuncDef;

extern OneWire_FuncDef DS18B20;

#endif


