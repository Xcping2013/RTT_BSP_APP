//#include "app_button.h"
//#include "app_sys_control.h"
#include "bsp_include.h"	
#include "app_include.h"

uint8_t buttonSTARTpressed=0;
uint8_t buttonRESETpressed=0;

//��ʼ�����͸�λ��������λ�����ƣ��ײ㲻��װ�κ����⶯��

//�����������Ժ������ơ���λ��һ�����  	//��ʼ�������� ��λ��������	
void buttonSTART_process(uint8_t inCh, uint8_t outCh)
{
	if(buttonRESETpressed==FALSE)																											//����ڸ�λ�У�START��Ч
	{
		if( getChInput(inCh)==IN_ON && buttonSTARTpressed==FALSE )					
		{
			delay_ms(20);		
			if( getChInput(inCh)==IN_ON )											 																//��ʼ����
			{
				 setChOutput(outCh,1);	 									
				 while(getChInput(inCh)==IN_ON) {;} 																						//�ȴ��ɿ�									 									 			
				 buttonSTARTpressed=TRUE;  
				 buttonRESETpressed=FALSE;			
				 CMD_TRACE("buttonSTART=1\n");
			}
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
		delay_ms(20);
		if( getChInput(inCh)==IN_ON )																				
		{		
			buttonRESETpressed=TRUE;	setChOutput(outCh,1);	 
			MotorAutoReset_preset();	
		}
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
			CMD_TRACE("enable button START for next test\n");		
		}
	}
	return 0;
}
	
