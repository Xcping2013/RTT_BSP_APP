#include "bsp_include.h"	
#include "app_include.h"

//#include "app_gpio_dido.h"	
//#include "app_sys_control.h"	
//#include "inc_mbtmc429.h"
//CubeMX txt

uint8_t	inputs_pin_num[INPUT_COUNT]=
{
	PB_5,	//IN1
	PB_6, //IN2
	PB_7, //IN3
	PB_8,	//IN4
	PB_9,	//IN5	---HOME0
	PE_0,	//IN6	---HOME1
	PE_1,	//IN7	---HOME2
	PE_2,	//IN8
	PE_3,	//IN9
	PE_4,	//IN10
	PE_5,	//IN11
	PE_6,	//IN12
	
//PB_1,
//PD_8,
//PD_12,
//PD_9,
//PB_15,
//PB_13,
};  
uint8_t	outputs_pin_num[OUTPUT_COUNT]=
{
	PA_15	 ,
	PC_10  ,
	PC_11	 ,
	PC_12	 ,
	PD_0	 ,
	PD_1	 ,
	PD_2   ,
	PD_3   ,
//	PB_14  ,
//	PD_10  ,
//	PD_13  ,
//	PD_11  ,
//	PE_14  ,
//	PE_15	 ,
};
uint8_t rgb_pin_num[3]={PD_6,PD_5,PD_4};
uint8_t homeSensorPin[3]={PB_9,PE_0,PE_1};

#if defined(USING_INC_MBTMC429) 

static rt_timer_t timer1;
static rt_uint8_t out_flash_on[9]={0};
static rt_uint16_t out_flash_delay[9]={0};
static rt_uint16_t out_flash_cnt[9]={0};
static int timer_10ms_init(void);
static void timeout1(void *parameter);

static void InitIn8AsExti(void);

void dido_hw_init(void)
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

#if defined(USING_IN8_EXIT)	
	InitIn8AsExti();
#endif
	
}
//
uint8_t getChInput(uint8_t channel)
{
	return rt_pin_read(inputs_pin_num[channel-1]) ? 1:0 ;
}
uint8_t getOutput(uint8_t channel)
{
	return OUT1_GPIO_Port->ODR & OUT1_Pin ? 1:0 ;
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
	CMD_TRACE("output toggle <ch> <delay_time>  - start or stop output toggle per time x10ms\n");
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
	buttonSTART_process(1,1);
	buttonRESET_process(2,2);
	//DEBUG_TRACE("timeout1 enter\n");	
	for(uint8_t i=0;i<9;i++)
	{
		if ((out_flash_on[i]==1) && (out_flash_cnt[i]++>out_flash_delay[i]))
		{
			if(i==8) pinToggle(rgb_pin_num[1]);
			else
			{
				pinToggle(outputs_pin_num[i]);
			}		
			//DEBUG_TRACE("out%d is flashing\n",i+1);	
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

//INIT_APP_EXPORT(dido_hw_init);
//MSH_CMD_EXPORT(readinput, read the state of the input channel);
//MSH_CMD_EXPORT(output_on, set the output channel on);
//MSH_CMD_EXPORT(output_off, set the output channel off);
//MSH_CMD_EXPORT(output_flashing, start or stop output flashing);
//MSH_CMD_EXPORT(rgb_pin_num, set the rgb_pin_num led);
/**/
//pressureAlarm

uint8_t pressureAlarm=0;
//HAL_NVIC_SetPriority(irqmap->irqno, 4, 0);  5-->4
void alarm_on(void *args)
{
	if(INPUT8==0 && buttonRESETpressed==0 )	 	
	{			
		HardStop(2);//HardStop(1);HardStop(0);
		//buttonRESETpressed=0;	KEY_RESET_LED=0;	KEY_START_pressed=0;	KEY_START_LED=0;
		homeInfo.Homed[2]=FALSE;
		homeInfo.GoHome[2]=FALSE;	
		homeInfo.GoLimit[2]=FALSE;	
		//pressureAlarm=1;
		CMD_TRACE("stop motor z due to pressure overhigh!!!\n");	
	} 
}

void InitIn8AsExti(void)
{
  rt_pin_attach_irq(inputs_pin_num[8-1], PIN_IRQ_MODE_FALLING, alarm_on, RT_NULL);
	rt_pin_irq_enable(inputs_pin_num[8-1], PIN_IRQ_ENABLE);
}






//
//int msh_gpio(int argc, char **argv)
//{
//    if (argc == 1)
//    {
//			CMD_TRACE("gpio get\n");
//			CMD_TRACE("  port:enum<A|a|B|b|C|c|D|d|E|e|F|f|G|g|H|h>\n");
//			CMD_TRACE("  pin:uint8<0,15>\n");
//			CMD_TRACE("gpio set");
//			CMD_TRACE("  port:enum<A|a|B|b|C|c|D|d|E|e|F|f|G|g|H|h>\n");
//			CMD_TRACE("  pin:uint8<0,15>\n");
//			CMD_TRACE("  state:enum<0|1>\n");
//			
//			CMD_TRACE("gpio config show\n");
//			CMD_TRACE("  port:enum<A|a|B|b|C|c|D|d|E|e|F|f|G|g|H|h>\n");
//			CMD_TRACE("  pin:uint8<0,15>\n");
//			
//			CMD_TRACE("gpio config\n");
//			CMD_TRACE("  port:enum<A|a|B|b|C|c|D|d|E|e|F|f|G|g|H|h>\n");
//			CMD_TRACE("  pin:uint8<0,15>\n");
//			CMD_TRACE("  mode:enum<input|outputPP|outputOD|altPP|altOD|analog|analogADC>(input)\n");
//			CMD_TRACE("  pullup:enum<none|up|down>(none)\n");
//			CMD_TRACE("  speed:enum<low|med|high|vhigh>(high)\n");		
//			CMD_TRACE("  altFunc:uint8<0,15>(0)\n");

//      return -RT_ERROR;
//    }
//    else
//    {

//    }
//    return 0;
//}

//MSH_CMD_EXPORT_ALIAS(msh_gpio, gpio,GPIO commands);



















//