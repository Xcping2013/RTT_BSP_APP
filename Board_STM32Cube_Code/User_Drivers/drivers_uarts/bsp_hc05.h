#ifndef __BSP_HC05_H
#define __BSP_HC05_H	 

#include "mcu_sys.h" 

//#define HC05_EN  			PAout(4) 		//��������KEY�ź�
//#define HC05_STATE  	PAin(15)		//��������״̬�ź�
  
u8 HC05_Init(void);
void HC05_CFG_CMD(u8 *str);
u8 HC05_Get_Role(void);
u8 HC05_Set_Cmd(u8* atstr);	    

#endif  
















