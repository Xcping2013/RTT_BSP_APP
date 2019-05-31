//#include "app_button.h"
//#include "app_sys_control.h"
#include "bsp_include.h"	
#include "app_include.h"


uint8_t buttonSTARTpressed=FALSE;
uint8_t buttonRESETpressed=FALSE;

uint8_t buttonSTARTCNT=0;
uint8_t buttonRESETCNT=0;
//��ʼ�����͸�λ��������λ�����ƣ��ײ㲻��װ�κ����⶯��

//�����������Ժ������ơ���λ��һ�����  	//��ʼ�������� ��λ��������	
void buttonSTART_process(uint8_t inCh, uint8_t outCh)
{
	if(buttonRESETpressed==FALSE)																											//����ڸ�λ�У�START��Ч
	{
		if( getChInput(inCh)==IN_ON && buttonSTARTpressed==FALSE )					
		{
			buttonSTARTCNT++;
			//delay_ms(20);		
			if( buttonSTARTCNT==2)											 																//��ʼ����
			{
				 setChOutput(outCh,1);	setChOutput(2,0);	
				 //while(getChInput(inCh)==IN_ON) {;} 																						//�ȴ��ɿ�									 									 			
				 buttonSTARTpressed=TRUE;  
				 buttonRESETpressed=FALSE;			
				 CMD_TRACE("buttonSTART=1\n");
			}
		}
		else if( getChInput(inCh)==IN_OFF)
		{
			buttonSTARTCNT=0;	
		}
	}
}
/*
��ԭ���ٶȴ�EEPROM����
Bx35_button: Y�������� ---> Z�����Ϻ��� ---> X���������

B445_button: Z�����Ϻ��»�ԭ�������������һ��λ�÷��ô�����ײ��Holder---> Y��������---> X���������

ROAD			 ��Z�����Ϻ���				
*/
void buttonRESET_process(uint8_t inCh, uint8_t outCh)
{
	if( getChInput(inCh)==IN_ON && buttonRESETpressed==FALSE )							
	{
		//delay_ms(20);
		buttonRESETCNT++;
		//if( getChInput(inCh)==IN_ON )				
		if( buttonRESETCNT==2)			
		{		
			buttonRESETpressed=TRUE;	setChOutput(outCh,1);	 setChOutput(1,0);	
			MotorAutoReset_preset();	
		}
	}
	else if( getChInput(inCh)==IN_OFF)
	{
		buttonRESETCNT=0;	
	}
}

int buttonSTART(int argc, char **argv)
{
	if(argc==2)
	{
		if (!strcmp(argv[1], "status"))	CMD_TRACE("buttonSTART=%d\n",buttonSTARTpressed);
		if (!strcmp(argv[1], "enable")) 
		{
			buttonSTARTpressed=FALSE;
			setChOutput(1,0);	
			CMD_TRACE("enable button START for next test\n",buttonSTARTpressed);		
		}
	}
	else
	{
		rt_kprintf("Usage: \n");
		//�û��ӿ�
		rt_kprintf("buttonSTART status          -button START is pressed or not\n");
		rt_kprintf("buttonSTART enable          -enable button START for next test\n");
	}
	return 0;
}
	
MSH_CMD_EXPORT(buttonSTART, status and enable );