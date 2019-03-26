/**
  \file		 Cmd_DIDO.c
  \version 1.00

  \brief DIDO Cmd access functions

  This file contains DIDO Cmd access functions.
*/
#include "usart.h"
#include "ctype.h"
#include "GPIO.h"

//ON[1]
//ON[1,2,3,4,5]
//OFF[1]
//OFF[1,2,3,4,5]
//INT[1]
//INT[1,2,3,4,5]

u16 LED_Blink=0;
/*******************************************************/
uint8_t  MyCLI_GetINT(char *Commands)
{
	if( strncmp(Commands,"int[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];			
		u8 channel=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0'	)
		{
			if(channel>0 && channel<=INTn)
			{
				printf("INT[%d]<%d>\r\n",channel,BSP_INT_GetState(channel-1) ?	0:1);
				return CMD_GET_OK;
			}
			else return CMD_PAR_ERR;
		}
		else return CMD_ERR;
	}
	else return CMD_NEXT;	
}

uint8_t  MyCLI_SetOUT(char *Commands)
{
	if( strncmp(Commands,"out[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];		
		u8 setval;		
		u8 channel=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='[' &&*(p+3)==']' && *(p+4)=='\0' )
		{
			if(channel>0 && channel<=OUTn)	
			{
				setval=*(p+2)-'0';
				if(setval==1)						{	BSP_OUT_On(channel-1);	}	
				else	if(setval==0)		  {	BSP_OUT_Off(channel-1);	}
				else return CMD_PAR_ERR;
				return CMD_SET_OK;
			}
			else return CMD_PAR_ERR;		
		}	
		else return CMD_ERR;
	}	
	else return CMD_NEXT;
}

//
uint8_t  MyCLI_SetLED(char *Commands)
{
	if( strncmp(Commands,"rgb[",4)==0  ) 
	{			
		if(Commands[5]==']'&& Commands[6]=='\0'	)
		{
			u8 led=Commands[4];
			switch(led)
			{
				case 'r':	LED_Blink &=~(BIT11|BIT12|BIT13);BSP_LED_On(LED_RED);	BSP_LED_Off(LED_GREEN);BSP_LED_Off(LED_BLUE); return CMD_SET_OK;
				case 'g':	LED_Blink &=~(BIT11|BIT12|BIT13);BSP_LED_On(LED_GREEN);BSP_LED_Off(LED_RED);BSP_LED_Off(LED_BLUE);	 	return CMD_SET_OK;
				case 'b':	LED_Blink &=~(BIT11|BIT12|BIT13);BSP_LED_On(LED_BLUE);	BSP_LED_Off(LED_RED);BSP_LED_Off(LED_GREEN);	return CMD_SET_OK;
				case '0':	LED_Blink &=~(BIT11|BIT12|BIT13); BSP_LED_Off(LED_BLUE);BSP_LED_Off(LED_RED);BSP_LED_Off(LED_GREEN);	return CMD_SET_OK;
				default : return CMD_PAR_ERR;							
			}

		}
		else return CMD_ERR;
	}	
	else if( strncmp(Commands,"blink[",6)==0  ) 
	{				
		if(Commands[7]==']'&& Commands[8]=='['&&Commands[10]==']'&&Commands[11]=='\0'	)
		{
			u8 led=Commands[6];
			u8 BlinkEn=Commands[9]-'0';
			switch(led)
			{
				case 'r':	if(BlinkEn)LED_Blink|=BIT11;else {LED_Blink&=~BIT11;BSP_LED_Off(LED_RED);} LED_Blink&=~BIT12;LED_Blink&=~BIT13;;BSP_LED_Off(LED_GREEN);BSP_LED_Off(LED_BLUE); return CMD_SET_OK;
				case 'g':	if(BlinkEn)LED_Blink|=BIT12;else {LED_Blink&=~BIT12;BSP_LED_Off(LED_GREEN);} LED_Blink&=~BIT13;LED_Blink&=~BIT11;BSP_LED_Off(LED_RED);BSP_LED_Off(LED_BLUE);	 return CMD_SET_OK;
				case 'b':	if(BlinkEn)LED_Blink|=BIT13;else {LED_Blink&=~BIT13;BSP_LED_Off(LED_BLUE);} LED_Blink&=~BIT11;LED_Blink&=~BIT12;BSP_LED_Off(LED_RED);BSP_LED_Off(LED_GREEN);	 return CMD_SET_OK;
				//case '0':	LED_Blink=0;			return CMD_SET_OK;
				case '1':	
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8': if(BlinkEn)LED_Blink|=1<<(led-'1');else {LED_Blink&=~(1<<(led-'1'));BSP_OUT_Off(led-'1');}  return CMD_SET_OK;
				default:  return CMD_PAR_ERR;
			}
		}
		else return CMD_ERR;
	}	
	else return CMD_NEXT;
}

