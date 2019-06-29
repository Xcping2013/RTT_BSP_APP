#include "dmm.h"
#include <stdlib.h>
#include "ad7124.h"
#include "ad7124_regs.h"
			
static int  dmm_fun_type	= DMM_FUNC_UNKNOW;

void ad7124_setKeyRegs(void)
{
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_0]);
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_1]);
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Channel_2]);

	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Config_1]);	
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_IOCon1]);
	
	ad7124_write_register(ad7124_handler,ad7124_regsW[AD7124_Config_2]);
}

void SetFuncViaADreg( int _Func )
{
	static uint8_t ad7124_inited=0;
	if(ad7124_inited==0)	
	{
		ad7124_init();
		ad7124_inited=1;
	}
	if( dmm_fun_type != _Func )
	{
		dmm_fun_type = _Func;

/* 
		AIN3、AIN4=U
		AIN6、AIN7=I
		AIN1、AIN2=R
		
		ad7124_regsW[AD7124_Filter_0].value=0x0070;
		ad7124_regsW[AD7124_Offset_0].value=0x0070;				
		ad7124_regsW[AD7124_Gain_0].value=0x0070;			
*/
		switch( _Func )
		{
			case DMM_FUNC_DCV_100mV:	//0.1/10*128=1.28V	PGA=128;
				
					 ad7124_regsW[AD7124_Channel_0].value=0x8064;	//AIN3、AIN4
					 ad7124_regsW[AD7124_Channel_1].value=0x00c7;	//DISABLE
					 ad7124_regsW[AD7124_Channel_2].value=0x0022;	//DISABLE		
					 ad7124_regsW[AD7124_Config_0].value=0x0077;	//PGA=128;				
		
				break;
			case DMM_FUNC_DCV_1V:			//1/10*16=1.6V		  PGA=16;
				
					 ad7124_regsW[AD7124_Channel_0].value=0x8064;	
					 ad7124_regsW[AD7124_Channel_1].value=0x00c7;	
					 ad7124_regsW[AD7124_Channel_2].value=0x0022;	
					 ad7124_regsW[AD7124_Config_0].value=0x0074;	
			
				break;
			case DMM_FUNC_DCV_10V:		//10/10*2=2v				PGA=2
				
					 ad7124_regsW[AD7124_Channel_0].value=0x8064;	
					 ad7124_regsW[AD7124_Channel_1].value=0x00c7;	
					 ad7124_regsW[AD7124_Channel_2].value=0x0022;	
					 ad7124_regsW[AD7124_Config_0].value=0x0071;	
			
				break;
			case DMM_FUNC_DCV_25V:		//25/10*1=2.5V			PGA=1
				
					 ad7124_regsW[AD7124_Channel_0].value=0x8064;	
					 ad7124_regsW[AD7124_Channel_1].value=0x00c7;	
					 ad7124_regsW[AD7124_Channel_2].value=0x0022;	
					 ad7124_regsW[AD7124_Config_0].value=0x0070;	
			
				break;

			case DMM_FUNC_DCI_100uA:	
				break;
			case DMM_FUNC_DCI_1mA:		
				break;
			case DMM_FUNC_DCI_100mA:	//0.15ohm *0.1 *128=1.92V		PGA=128
				
					 ad7124_regsW[AD7124_Channel_0].value=0x0064;										
					 ad7124_regsW[AD7124_Channel_1].value=0x90c7;													
				   ad7124_regsW[AD7124_Channel_2].value=0x0022;												
			
					 ad7124_regsW[AD7124_Config_1].value=0x0077;												
				
				break;
			case DMM_FUNC_DCI_1A:			//0.15ohm *1 *16=2.4V				PGA=16
					 ad7124_regsW[AD7124_Channel_0].value=0x0064;										
					 ad7124_regsW[AD7124_Channel_1].value=0x90c7;													
				   ad7124_regsW[AD7124_Channel_2].value=0x0022;												
			
					 ad7124_regsW[AD7124_Config_1].value=0x0074;
				break;

			case DMM_FUNC_OHM_10R_4W:	//10ohm *0.001 *128=1.28V		PGA=128
				
					ad7124_regsW[AD7124_IOCon1].value=0x0700;			//IOUT0=1000uA
					ad7124_regsW[AD7124_Config_2].value=0x0077;		//1*0.001*128=0.128V
			
					ad7124_regsW[AD7124_Channel_0].value=0x0064;
					ad7124_regsW[AD7124_Channel_1].value=0x00c7;		
					ad7124_regsW[AD7124_Channel_2].value=0xA022;	//1010000000100010	AIN1	AIN2								
				
				break;	
			case DMM_FUNC_OHM_1K_4W:	//1000ohm *0.001 *2=2V			PGA=2
				
					ad7124_regsW[AD7124_IOCon1].value=0x0700;				//IOUT0=1000uA
					ad7124_regsW[AD7124_Config_2].value=0x0071;		
			
					ad7124_regsW[AD7124_Channel_0].value=0x0064;
					ad7124_regsW[AD7124_Channel_1].value=0x00c7;		
					ad7124_regsW[AD7124_Channel_2].value=0xA022;	//1010000000100010	AIN1	AIN2			
			
				break;
			case DMM_FUNC_OHM_10K_4W:	//10kohm *0.0001 *2=2V			PGA=2
				
					ad7124_regsW[AD7124_IOCon1].value=0x0200;				//IOUT0=1000uA
					ad7124_regsW[AD7124_Config_2].value=0x0071;		
			
					ad7124_regsW[AD7124_Channel_0].value=0x0064;
					ad7124_regsW[AD7124_Channel_1].value=0x00c7;		
					ad7124_regsW[AD7124_Channel_2].value=0xA022;	//1010000000100010	AIN1	AIN2		
			
				break;
			default:
				break;
		}
		ad7124_setKeyRegs();
		AD7124_ReadAverData(ad7124_handler);
		AD7124_ReadAverData(ad7124_handler);
		AD7124_ReadAverData(ad7124_handler);
	}
}
//
void print_dmm_value( int _Func )
{
	SetFuncViaADreg(_Func);
	if(_Func==DMM_FUNC_DCV_100mV || _Func==DMM_FUNC_DCV_1V||_Func==DMM_FUNC_DCV_10V || _Func==DMM_FUNC_DCV_25V)
	{
		double voltage;
		uint8_t voltageStr[10];
		voltage=AD7124_ReadAverData(ad7124_handler);
		sprintf(voltageStr,"%lf",voltage*KTable[_Func]-CTable[_Func]);			
		AD7124_TRACE("voltage=%s uV\n",voltageStr);				
	}
	if(_Func==DMM_FUNC_DCI_100uA || _Func==DMM_FUNC_DCI_1mA||_Func==DMM_FUNC_DCI_100mA || _Func==DMM_FUNC_DCI_1A)
	{
		double voltage;
		uint8_t voltageStr[10];
		voltage=AD7124_ReadAverData(ad7124_handler);	
		sprintf(voltageStr,"%lf",voltage*KTable[_Func]-CTable[_Func]);					
		AD7124_TRACE("Current=%s mA\n",voltageStr);			
	}	
	if(_Func==DMM_FUNC_OHM_10R_4W || _Func==DMM_FUNC_OHM_1K_4W||_Func==DMM_FUNC_OHM_10K_4W )
	{
		double voltage;
		uint8_t voltageStr[10];
		voltage=AD7124_ReadAverData(ad7124_handler);	
		sprintf(voltageStr,"%lf",voltage*KTable[_Func]-CTable[_Func]);					
		AD7124_TRACE("resistor=%s mA\n",voltageStr);			
	}	
}
void ad7124_getSetK(void)
{
	 ad7124_regsW[AD7124_Channel_0].value=0x8064;	
	 ad7124_regsW[AD7124_Channel_1].value=0x00c7;	
	 ad7124_regsW[AD7124_Channel_2].value=0x0022;	
	 ad7124_regsW[AD7124_Config_0].value=0x0070;	
	 ad7124_setKeyRegs();
	 AD7124_ReadAverData(ad7124_handler);
	 AD7124_ReadAverData(ad7124_handler);
	 AD7124_ReadAverData(ad7124_handler);
	 double voltage=AD7124_ReadAverData(ad7124_handler);
	 if(voltage!=0) 
	 {
		 voltage=2500000/voltage;
		 KTable[DMM_FUNC_DCV_100mV]=voltage/128;
		 KTable[DMM_FUNC_DCV_1V]=voltage/16;
		 KTable[DMM_FUNC_DCV_10V]=voltage/2;
		 KTable[DMM_FUNC_DCV_25V]=voltage; 
	 }
	 ad7124_regsW[AD7124_Channel_0].value=0x0064;										
	 ad7124_regsW[AD7124_Channel_1].value=0x90c7;													
	 ad7124_regsW[AD7124_Channel_2].value=0x0022;												
	 ad7124_regsW[AD7124_Config_1].value=0x0077;	
	 ad7124_setKeyRegs();
	 AD7124_ReadAverData(ad7124_handler);
	 AD7124_ReadAverData(ad7124_handler);
	 AD7124_ReadAverData(ad7124_handler); 
	 voltage=AD7124_ReadAverData(ad7124_handler);
	 if(voltage!=0) 
	 {
		 voltage=375000/voltage;
		 KTable[DMM_FUNC_DCI_100mA]=voltage;
		 KTable[DMM_FUNC_DCI_1A]=voltage/8;
	 }		
	  pinMode(PC_6,GPIO_MODE_OUTPUT_OD);
	  pinReset(PC_6);
		ad7124_regsW[AD7124_IOCon1].value=0x0700;			//IOUT0=1000uA
		ad7124_regsW[AD7124_Config_2].value=0x0077;		//1*0.001*128=0.128V
		ad7124_regsW[AD7124_Channel_0].value=0x0064;
		ad7124_regsW[AD7124_Channel_1].value=0x00c7;		
		ad7124_regsW[AD7124_Channel_2].value=0xA022;	//1010000000100010	AIN1	AIN2								
	  ad7124_setKeyRegs();
	  AD7124_ReadAverData(ad7124_handler);
	  AD7124_ReadAverData(ad7124_handler);
	  AD7124_ReadAverData(ad7124_handler); 
	  voltage=AD7124_ReadAverData(ad7124_handler);
	  if(voltage!=0) 
	  {
			/*
			voltage=10ohm *0.001 *128=k*10ohm *0.001 *2
						 =
			*/
		  voltage=10000/voltage;		
		  KTable[DMM_FUNC_OHM_10R_4W]=voltage;
		  KTable[DMM_FUNC_OHM_1K_4W]=voltage/64;
			KTable[DMM_FUNC_OHM_10K_4W]=voltage/640;
	  }	
		pinReset(PC_6);		
		Cal_RamToFlash();
}

