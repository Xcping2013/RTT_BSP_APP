#include "74HC595.h"
#include "delay.h"

#define REGISTER_CNT 		2 
 
#define STORAGE_CP   		PBout(13) 
#define SHIFT_CP  	  	PBout(15)
#define OUT_EN		   		PDout(9) 
#define DSERIAL					PDout(12)

static u8 stateArrOut[8*REGISTER_CNT] = {0};
	
static void updateRegister(void);
static void updateOutput(void);
static void HC595_Init(void);
static void HC595_SetByte(u8 byte);

void writeArray(void);
void clearstateArray(void);

static void HC595_Init(void)
{
	OUT_EN=0;
	HC595_SetByte(0x00); // Reset the values of the registers to 0  
	//MASTER_RESSET=RESET;
}
//Pulses the register
static void updateRegister(void)
{
	SHIFT_CP	= RESET;
	delay_us(2);
	SHIFT_CP	=	SET;
}
//Updates the output register
static void updateOutput(void)
{
	STORAGE_CP	= RESET;
	delay_us(2);
	STORAGE_CP	=	SET;
}
//Writes a byte to the shift registers
static void HC595_SetByte(u8 byte)
{
	for(u8 i = 0; i<8*REGISTER_CNT; i++)
	{
		DSERIAL = (byte & 0x01<<i)>>i;
		updateRegister();    
  } 
	updateOutput();
}
//zero the status buffer
void clearstateArray(void)
{
	for(u8 i = 0; i < 8*REGISTER_CNT; i++)
	{
		stateArrOut[i] = 0;
	}
}
//shitf out buffer bit after bit
void writeArray(void)
{
	for(u8 i = 8*REGISTER_CNT; i >0; i--)
	{
		DSERIAL = stateArrOut[i-1] & 0x01;
		delay_us(2);
		updateRegister();
	}  
	updateOutput();	
}
//reset the buffer and shift out
void HC595_WriteBitAtPos(u8 pin, u8 state)
{
	static u8 Inited_HC595;
	
	if(Inited_HC595==0)	{HC595_Init(); Inited_HC595=1;}	
	if(pin < 8*REGISTER_CNT)
	{
		stateArrOut[pin] = state; //设置对应位置数据状态       
	}
	writeArray();		 //ACT
}







