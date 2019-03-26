#include "board_debug.h"	
#include "delay.h"
#include "gpio.h"
#include "at24cxx.h"
#include "motor_motion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "INC_MBTMC429.h"

#include <rtthread.h>


static void hw_in_out_debug(void)
{
	u8 ret_ok=1;
													if(INPUT1==1) 	{rt_kprintf("input1 or rgb_5v ............err\r\n");ret_ok=0;}
	RGB_R=1;RGB_G=0;RGB_B=0;delay_ms(10);
													if(INPUT2==1) 	{rt_kprintf("input2 or rgb_r .............err\r\n"); ret_ok=0;}
													if(INPUT3==0) 	{rt_kprintf("input3 or rgb_g .............err\r\n"); ret_ok=0;}
													if(INPUT4==0) 	{rt_kprintf("input4 or rgb_b .............err\r\n"); ret_ok=0;}
	RGB_R=0;RGB_G=1;RGB_B=0;delay_ms(10);										        
													if(INPUT2==0) 	{rt_kprintf("input2 or rgb_r .............err\r\n"); ret_ok=0;}
													if(INPUT3==1) 	{rt_kprintf("input3 or rgb_g .............err\r\n"); ret_ok=0;}
													if(INPUT4==0) 	{rt_kprintf("input4 or rgb_b .............err\r\n"); ret_ok=0;}
	RGB_R=0;RGB_G=0;RGB_B=1;delay_ms(10);												     
													if(INPUT2==0) 	{rt_kprintf("input2 or rgb_r .............err\r\n"); ret_ok=0;}
													if(INPUT3==0) 	{rt_kprintf("input3 or rgb_g .............err\r\n"); ret_ok=0;}
													if(INPUT4==1) 	{rt_kprintf("input4 or rgb_b .............err\r\n"); ret_ok=0;}	
														
	OUTPUT1=1;delay_ms(10);	if(INPUT5==1) 	{rt_kprintf("input5 or output1 ...........err\r\n");		ret_ok=0;}
	OUTPUT1=0;delay_ms(10);	if(INPUT5==0) 	{rt_kprintf("input5 or output1 ...........err\r\n");		ret_ok=0;}
	                                                                  
	OUTPUT2=1;delay_ms(10);	if(INPUT6==1) 	{rt_kprintf("input6 or output2 ...........err\r\n");   ret_ok=0;}
	OUTPUT2=0;delay_ms(10);	if(INPUT6==0) 	{rt_kprintf("input6 or output2 ...........err\r\n");   ret_ok=0;}
	                                                                   
	OUTPUT3=1;delay_ms(10);	if(INPUT7==1) 	{rt_kprintf("input7 or output3 ...........err\r\n");   ret_ok=0;}
	OUTPUT3=0;delay_ms(10);	if(INPUT7==0) 	{rt_kprintf("input7 or output3 ...........err\r\n");   ret_ok=0;}
	                                                                  
	OUTPUT4=1;delay_ms(10);	if(INPUT8==1) 	{rt_kprintf("input8 or output4 ...........err\r\n");   ret_ok=0;}
	OUTPUT4=0;delay_ms(10);	if(INPUT8==0) 	{rt_kprintf("input8 or output4 ...........err\r\n");   ret_ok=0;}
	          
	OUTPUT5=1;delay_ms(10);	if(INPUT9==1) 	{rt_kprintf("input9 or output5 ...........err\r\n");   ret_ok=0;}
	OUTPUT5=0;delay_ms(10);	if(INPUT9==0) 	{rt_kprintf("input9 or output5 ...........err\r\n");   ret_ok=0;}
            
	OUTPUT6=1;delay_ms(10);	if(INPUT10==1) 	{rt_kprintf("input10 or output6 ..........err\r\n");   ret_ok=0;}
	OUTPUT6=0;delay_ms(10);	if(INPUT10==0) 	{rt_kprintf("input10 or output6 ..........err\r\n");   ret_ok=0;}
	          
	OUTPUT7=1;delay_ms(10);	if(INPUT11==1) 	{rt_kprintf("input11 or output7 ..........err\r\n");   ret_ok=0;}
	OUTPUT7=0;delay_ms(10);	if(INPUT11==0) 	{rt_kprintf("input11 or output7 ..........err\r\n");   ret_ok=0;}
																									 
	OUTPUT8=1;delay_ms(10);	if(INPUT12==1) 	{rt_kprintf("input12 or output8 ..........err\r\n");   ret_ok=0;}   
	OUTPUT8=0;delay_ms(10);	if(INPUT12==0) 	{rt_kprintf("input12 or output8 ..........err\r\n");   ret_ok=0;}   
	if(ret_ok)	rt_kprintf("inputs and outputs test......ok\r\n");	
	
}

static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}
static void hw_eeprom_debug(void)
{
	u8 *save_info="this page data save for eeprom test";
	u8 read_info[64];
	u8 len=strlen(save_info);
					
	rt_kprintf("eeprom test..................");
	at24cxx.write(at24c256,100,save_info,len);
	delay_ms(10);
	at24cxx.read(at24c256 , 100, read_info, len);
	
  if(Buffercmp((uint8_t*)save_info,(uint8_t*)read_info,len))
  {
    rt_kprintf("err\r\n");
  }	
	else rt_kprintf("ok\r\n");
	
}

static void hw_motor_debug(void)
{
	rt_kprintf("tmc429 test..................");
	if(Read429SingleByte(IDX_REF_SWITCHES, 3)==0) 
		rt_kprintf("err");

	else 
	{		

		rt_kprintf("ok\r\n\r\n最后:检测限位采集电路");
		rt_kprintf("\r\n先发送motor_get(1,\"L\")采集数据再把L1接GND后发送motor_get(1,\"L\")返回值有变化才ok\r\n");	

	}
}

void board_debug(void)
{
	rt_kprintf("\r\n");
	hw_in_out_debug();
	hw_eeprom_debug();
	hw_motor_debug();
	
}

static u8 key_flag[2]={0};
static u8 key_lock[2]={0};
u8 auto_test_mode=0;
void motor_cyliner_test(void)
{
	
//	while(1)
	{
		if(INPUT11==0) 
		{
			delay_ms(20);
			if(INPUT11==0) 
			{
				key_flag[0]=1;	
			}
			else key_flag[0]=0;
		}
		else key_flag[0]=0;
		
		if(INPUT10==0) 
		{
			delay_ms(20);
			if(INPUT10==0) 
			{
				key_flag[1]=1;	
			} 
			else key_flag[1]=0;
		}
		else key_flag[1]=0;
		
		if(key_flag[0]==1 && key_lock[0]==0)
		{
			OUTPUT4=~OUTPUT4;
			SpeedChangedFlag[0]=TRUE;
			Set429RampMode(0, RM_VELOCITY);
			Write429Short(IDX_VMAX|0<<5, 2047);
			Write429Short(IDX_VTARGET|0<<5, 500);
			key_lock[0]=1;
		}
		else if(key_flag[0]==0 && key_flag[1]==0)
		{
			HardStop(0); key_lock[0]=0;
		}
		if(key_flag[1]==1&& key_lock[1]==0) 
		{
			OUTPUT4=~OUTPUT4;
			SpeedChangedFlag[0]=TRUE;
			Set429RampMode(0, RM_VELOCITY);
			Write429Short(IDX_VMAX|0<<5, 2047);
			Write429Short(IDX_VTARGET|0<<5, -500);
			key_lock[1]=1;
		}
			
		else if(key_flag[0]==0 && key_flag[1]==0)
		{
			HardStop(0); key_lock[1]=0;
		}
	}
}
void add_test_mode(uint8_t type, uint8_t enable)
{
	switch(type)
	{
		case 1:	if(enable) 
						{
							OUTPUT5=1;OUTPUT6=1;
							auto_test_mode=1;
//							Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR|IFCONF_INV_REF);

							at24cxx.writeU8(at24c256,ADDR_TEST_MODE,auto_test_mode);
						}
						else 
						{
							OUTPUT5=0;OUTPUT6=0;
							auto_test_mode=0;
//							Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_EN_REFR|IFCONF_SDO_INT|IFCONF_INV_DIR);

							at24cxx.writeU8(at24c256,ADDR_TEST_MODE,auto_test_mode);
						}
			break;					
			
		default:
			break;
	}
}

