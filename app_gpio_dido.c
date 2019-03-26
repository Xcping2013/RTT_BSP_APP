#include "bsp_include.h"	
#include "app_include.h"

//#include "app_gpio_dido.h"	
//#include "app_sys_control.h"	
//#include "inc_mbtmc429.h"
//CubeMX txt
uint8_t inputs_pin_num[12]={91,92,93,95,96,97,98,1,2,3,4,5};
uint8_t outputs_pin_num[8]={77,78,79,80,81,82,83,84};
uint8_t rgb_pin_num[3]={85,86,87};
uint8_t homeSensorPin[3]={96,97,98};

#if defined(USING_INC_MBTMC429) 

static rt_timer_t timer1;
static rt_uint8_t out_flash_on[9]={0};
static rt_uint16_t out_flash_delay[9]={0};
static rt_uint16_t out_flash_cnt[9]={0};
static int timer_10ms_init(void);
static void timeout1(void *parameter);

void dido_gpio_init(void)
{    
	uint8_t i;
	for(i=0;i<12;i++)
	{
    rt_pin_mode(inputs_pin_num[i], PIN_MODE_INPUT_PULLUP); 
	}
	for(i=0;i<8;i++)
	{
    rt_pin_mode(outputs_pin_num[i], PIN_MODE_OUTPUT); 
		rt_pin_write(outputs_pin_num[i],PIN_LOW);
	}   
	for(i=0;i<3;i++)
	{
    rt_pin_mode(rgb_pin_num[i], PIN_MODE_OUTPUT); 
		rt_pin_write(rgb_pin_num[i],PIN_LOW);
	} 
	timer_10ms_init();
}
//
uint8_t getChInput(uint8_t channel)
{
	return rt_pin_read(inputs_pin_num[channel-1]) ? 1:0 ;
}
void setChOutput(uint8_t channel, uint8_t setval)
{
	rt_pin_write(outputs_pin_num[channel-1],setval);
}
int readinput(int argc, char **argv)
{
    if (argc == 1)
    {
        CMD_TRACE("Please input: readinput <ch1> <ch2> ......\n");
        return -RT_ERROR;
    }
    else
    {
			uint8_t channel,i;
			for(i=1;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 13)
				{
					if((argc-i)>1) CMD_TRACE("in[%d]=%d,",channel,rt_pin_read(inputs_pin_num[channel-1]) ? 0:1 );
					else           CMD_TRACE("in[%d]=%d",channel,rt_pin_read(inputs_pin_num[channel-1]) ? 0:1 );
				}
				else 
				{		
					CMD_TRACE("input channel must be 1~12\n");
					return RT_ERROR;
				}
			}
			CMD_TRACE("\n");
    }
    return 0;
}
int output(int argc, char **argv)
{
	if (argc > 2)
	{
		if (!strcmp(argv[1], "on")) 
		{	
			uint8_t channel,i;
			for(i=2;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 9)
				{
					rt_pin_write(outputs_pin_num[channel-1],PIN_HIGH);
					if((argc-i)>1)  CMD_TRACE("out[%d]=1,",channel); 
					else           	CMD_TRACE("out[%d]=1",channel);
					
				}
				else 
				{		
					CMD_TRACE("output channel must be 1~8\n");
					return RT_ERROR;
				}
			}
			CMD_TRACE("\n");
			return RT_EOK;
		}
		else if (!strcmp(argv[1], "off")) 
		{	
			uint8_t channel,i;
			for(i=2;argc>i;i++)
			{
				channel=atoi(argv[i]);
				if(0<channel && channel< 9)
				{
					rt_pin_write(outputs_pin_num[channel-1],PIN_LOW);
					if((argc-i)>1)  CMD_TRACE("out[%d]=0,",channel); 
					else           	CMD_TRACE("out[%d]=0",channel);
				}
				else 
				{		
					CMD_TRACE("output channel must be 1~8\n");
					return RT_ERROR;
				}
			}
			CMD_TRACE("\n");
			return RT_EOK;
		}
	}
	if (argc == 4)
	{
		if (!strcmp(argv[1], "toggle"))
		{
			rt_uint8_t flash_ch=atoi(argv[2]);
			rt_uint8_t Delaycnt=atoi(argv[3]);
			if(0<flash_ch && flash_ch< 9)
			{
				if(Delaycnt)
				{
					out_flash_on[flash_ch-1]=1;
					out_flash_delay[flash_ch-1]=Delaycnt;							 
					CMD_TRACE("out[%d] toggle every %dx10ms\n",flash_ch,Delaycnt);
				}
				else
				{
					out_flash_on[flash_ch-1]=0;
					rt_pin_write(outputs_pin_num[flash_ch-1],PIN_LOW);
					CMD_TRACE("out[%d] stop toggle\n",flash_ch);		
				}
				return RT_EOK;
			}
			else 
			{		
				CMD_TRACE("output channel must be 1~8\n");
				return RT_ERROR;
			}
		}
	}
	CMD_TRACE("Usage: \n");
	CMD_TRACE("output on <ch>  - set the output channel on\n");
	CMD_TRACE("output off <ch>     - set the output channel off\n");
	CMD_TRACE("output toggle <delay_time>  - start or stop output toggle per time x10ms\n");
	return -RT_ERROR;
}

static int timer_10ms_init(void)
{
	timer1 = rt_timer_create("timer1", timeout1,
                             RT_NULL, 3,
                             RT_TIMER_FLAG_PERIODIC);
  if (timer1 != RT_NULL) 
		rt_timer_start(timer1);	
	return 0;
}
static void timeout1(void *parameter)
{
	//buttonSTART_process(1,1);
	buttonRESET_process(2,2);
	for(uint8_t i=0;i<9;i++)
	{
		if ((out_flash_on[i]==1) && (out_flash_cnt[i]++>out_flash_delay[i]))
		{
			if(i==8) pinToggle(rgb_pin_num[1]);
			else
			{
				pinToggle(outputs_pin_num[i]);
			}		
			DEBUG_TRACE("out%d is flashing\n",i+1);	
			out_flash_cnt[i]=0;
		}
	}	
}
//
int beep(int argc, char **argv)
{
	if (argc == 2)
	{
		if (!strcmp(argv[1], "on"))  	{rt_pin_write(rgb_pin_num[1],1);CMD_TRACE("beep on\n");	return RT_EOK;}
		if (!strcmp(argv[1], "off"))  {rt_pin_write(rgb_pin_num[1],0);CMD_TRACE("beep off\n");	return RT_EOK;}
	}
	else if (argc == 3)
	{
		if (!strcmp(argv[1], "delay")) 
		{	
			out_flash_delay[8]=atoi(argv[2]);
			if(out_flash_delay[8]) { out_flash_on[8]=1; CMD_TRACE("beep delay on\n");}
			else 									 { rt_pin_write(rgb_pin_num[1],0);out_flash_on[8]=0;}
			return RT_EOK;
		}
	}

	CMD_TRACE("Usage: \n");
	CMD_TRACE("beep on   - set beep on\n");
	CMD_TRACE("beep off     - set beep off\n");
	CMD_TRACE("beep delay <delay_time>  - set beep on per time x10ms\n");
	return -RT_ERROR;
}

int rgb(int argc, char **argv)
{
	if (argc != 2)
	{
		CMD_TRACE("Please input: rgb <r|g|b|0>......\n");
		return -1;
	}
  else
  {
		//char *rgbtemp=argv[1];
//		switch(rgbtemp[0])
		switch(argv[1][0])
		{
			case 'r':	rt_pin_write(rgb_pin_num[0],1);rt_pin_write(rgb_pin_num[1],0);rt_pin_write(rgb_pin_num[2],0);
				break;
			case 'g':	rt_pin_write(rgb_pin_num[0],0);rt_pin_write(rgb_pin_num[1],1);rt_pin_write(rgb_pin_num[2],0);
				break;
			case 'b':	rt_pin_write(rgb_pin_num[0],0);rt_pin_write(rgb_pin_num[1],0);rt_pin_write(rgb_pin_num[2],1);
				break;
			default:	rt_pin_write(rgb_pin_num[0],0);rt_pin_write(rgb_pin_num[1],0);rt_pin_write(rgb_pin_num[2],0);
				break;		
		}
		CMD_TRACE("rgb=%c\n",argv[1][0]);
	}
	return 0;
}
//
//

//
#endif

//INIT_APP_EXPORT(dido_gpio_init);
//MSH_CMD_EXPORT(readinput, read the state of the input channel);
//MSH_CMD_EXPORT(output_on, set the output channel on);
//MSH_CMD_EXPORT(output_off, set the output channel off);
//MSH_CMD_EXPORT(output_flashing, start or stop output flashing);
//MSH_CMD_EXPORT(rgb_pin_num, set the rgb_pin_num led);


