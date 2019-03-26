#include "74HC165.h"
#include "delay.h"

#define REGISTER_CNT 		2

#define PARALLEL_LOAD   PBout(14) 
#define SHIFT_CLK  	  	PDout(10) 
#define BIT_QH		   		PDin(11)  
#define CLK_EN		   		PDout(13) 

static volatile char stateArrInt[8*REGISTER_CNT] = {0};

void HC165_Init(void)
{
	CLK_EN=RESET;
	PARALLEL_LOAD=SET;
	SHIFT_CLK=SET;
}
//Load inputs and shift in bit after bit to buffer
void HC165_ReadDatas(void)
{
	PARALLEL_LOAD	= RESET;
	delay_us(5);
	PARALLEL_LOAD	=	SET;
	delay_us(5);
	
	for(u8 i = (8*REGISTER_CNT); i > 0; i--)
	{
		stateArrInt[i-1]=BIT_QH; 
		
		SHIFT_CLK=RESET;
		delay_us(5);
		SHIFT_CLK=SET;		
		delay_us(5);
		
	}  
}


//shift in and return the buffer 
u8 HC165_ReadBitAtPos(unsigned char pin)
{
	static u8 Inited_HC165;
	if(Inited_HC165==0)	{HC165_Init(); Inited_HC165=1;}
	
	HC165_ReadDatas();
	return stateArrInt[pin];
}

