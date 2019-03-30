#ifndef __BSP_HC05_H
#define __BSP_HC05_H	 

#include "bsp_defines.h" 

u8 HC05_Init(void);
void HC05_CFG_CMD(u8 *str);
u8 HC05_Get_Role(void);
u8 HC05_Set_Cmd(u8* atstr);	    

#endif  
















