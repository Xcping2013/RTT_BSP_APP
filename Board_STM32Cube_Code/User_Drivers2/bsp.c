#include "bsp.h"

void bsp_Init(void)
{	
	uart1_hw_init(115200);
	at24cxx_hw_init();

}

