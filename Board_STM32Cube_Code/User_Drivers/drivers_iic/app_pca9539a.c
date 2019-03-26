#include "bsp.h"
//#include "app_pca9539a.h"

#if 1
    #define PCA9539A_TRACE         rt_kprintf
#else
    #define PCA9539A_TRACE(...)
#endif

static void PCA9539A_Init(pca9539a_t dev)
{
	SoftI2c.pin_init(dev.pins);
	
	if (SoftI2c.check(dev.pins,dev.devAddress) == 1)
	{
		PCA9539A_TRACE("iic device pca9539a addr=%d......no found\n",dev.devAddress);
	}
	else
	{
		PCA9539A_TRACE("iic device pca9539a addr=%d......found\n",dev.devAddress);

//user test		
//		PCA9539A_TRACE("dev.out_data=%d\n",dev.out_data);
//		PCA9539A_TRACE("&dev.out_data=%d\n",&dev.out_data);
//		PCA9539A_TRACE("(uint8_t *)dev.out_data=%d\n",(uint8_t *)dev.out_data);
//		PCA9539A_TRACE("(uint8_t *)1=%d\n",(uint8_t *)1);	
		u8 reg_data=0xff; SoftI2c.writes(dev.pins,1,dev.devAddress,6,&reg_data,1);		 //Set p0 as input
		
		reg_data=0;		 SoftI2c.writes(dev.pins,1,dev.devAddress,4,&reg_data,1);		 //set input Polarity
		reg_data=0;		 SoftI2c.writes(dev.pins,1,dev.devAddress,7,&reg_data,1);		 //set p1 as output
									 SoftI2c.writes(dev.pins,1,dev.devAddress,3,&dev.out_data,1);//set all outputs off
	}
}

/******************--->	INPUT & OUTPUT	**************************
 */
pca9539a_t pca9539a_1=
{
	{&PC14,&PC15},
	0xE8,
	0,
	0,
};
pca9539a_t pca9539a_2=
{
	{&PC14,&PC15},
	0xEC,
	0,
	0,
};
void dido_hw_init(void)
{
	PCA9539A_Init(pca9539a_1);
	PCA9539A_Init(pca9539a_2);
}

uint8_t  CMD_int(char *Commands)
{
	if( strcmp(Commands,"inputs")==0  ) 
	{
		uint8_t i;
		char data_read[16]="0000000000000000";
		SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
		SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
		for(i=0;i<8;i++)
		{
			if(pca9539a_1.in_data & (1 << i))	data_read[i]='1'; else data_read[i]='0';
			if(pca9539a_2.in_data & (1 << i))	data_read[i+8]='1'; else data_read[i+8]='0';	
		}
		PCA9539A_TRACE("<%.16s>",data_read);
		return 1;
	}
	else if( strncmp(Commands,"int[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];			
		u8 channel=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0'	&& channel>0 && channel<17)
		{	
//			PCA9539A_TRACE("pca9539a_1.in_data=%d,pca9539a_2.in_data=%d\n",pca9539a_1.in_data,pca9539a_2.in_data);
			SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
			SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
//			PCA9539A_TRACE("pca9539a_1.in_data=%d,pca9539a_2.in_data=%d\n",pca9539a_1.in_data,pca9539a_2.in_data);
			PCA9539A_TRACE("<%d>", channel<9 ? (pca9539a_1.in_data & (1 << (channel-1))? 1:0):(pca9539a_2.in_data & (1 << (channel-9))? 1:0));	
				
			return 1;
		}
	}
  else return 0;	
}
//
uint8_t  CMD_out(char *Commands)
{
	if( strncmp(Commands,"out[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];		
		u8 setval;		
		u8 channel=strtol(s, &p, 10);	

		if(*p==']'&& *(p+1)=='[' && channel>0 && channel<=16)	
		{
			s=NULL;
			setval=strtol(p+2, &s, 10);	
			if(*(s)==']'&& *(s+1)=='\0' )
			{
				if(channel<9)	
				{
					pca9539a_1.out_data= setval? (pca9539a_1.out_data | (1 << (channel-1))) :(pca9539a_1.out_data & ((uint8_t) ~(1 << (channel-1))));
					SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
				}
				else
				{
					pca9539a_2.out_data= setval? (pca9539a_2.out_data | (1 << (16-channel))) :(pca9539a_2.out_data & ((uint8_t) ~(1 << (16-channel))));
					SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
				}	
				//PCA9539A_TRACE("pca9539a_1.out_data=%d,pca9539a_2.out_data=%d\n",pca9539a_1.out_data,pca9539a_2.out_data);			
				PCA9539A_TRACE("<OK>");
				return 1;
			}			
		}	
	}	
  else return 0;	 
}
/******************			INPUT & OUTPUT<---**************************
*/

