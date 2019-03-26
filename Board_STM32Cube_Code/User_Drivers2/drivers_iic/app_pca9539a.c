#include "bsp.h"

//#include "app_pca9539a.h"

#define PCA9539A_DEBUG
#ifdef  PCA9539A_DEBUG
    #define PCA9539A_TRACE         printf
#else
    #define PCA9539A_TRACE(...)
#endif

static void PCA9539A_Init(pca9539a_t dev)
{
	SoftI2c.pin_init(dev.pins);
	
	if (SoftI2c.check(dev.pins,dev.devAddress) == 1)
	{
		PCA9539A_TRACE("no pca9539a found\r\n");
	}
	else
	{
		PCA9539A_TRACE("find iic device pca9539a device addr=%d\r\n",dev.devAddress);
		//Set po as input
		SoftI2c.writes(dev.pins,1,dev.devAddress,6,(uint8_t *)1,1);
		//set input Polarity
		SoftI2c.writes(dev.pins,1,dev.devAddress,4,(uint8_t *)0,1);
		//set p1 as output
		SoftI2c.writes(dev.pins,1,dev.devAddress,7,(uint8_t *)0,1);
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
		printf("<%.16s>",data_read);
		return 1;
	}
	else if( strncmp(Commands,"int[",4)==0  ) 
	{	
		char *p = NULL;
		char *s = &Commands[4];			
		u8 channel=strtol(s, &p, 10);	
		if(*p==']'&& *(p+1)=='\0'	&& channel>0 && channel<17)
		{	
			SoftI2c.reads(pca9539a_1.pins,1,pca9539a_1.devAddress,0, &pca9539a_1.in_data ,1);	
			SoftI2c.reads(pca9539a_2.pins,1,pca9539a_2.devAddress,0, &pca9539a_2.in_data ,1);	
			printf("<%d>", channel<9 ? pca9539a_1.in_data & (1 << (channel-1)):pca9539a_2.in_data & (1 << (channel-9)));	
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
					pca9539a_1.out_data= setval? (pca9539a_1.out_data | (1 << (channel-1))) :(pca9539a_1.out_data & (1 << (channel-1)));
					SoftI2c.writes(pca9539a_1.pins,1,pca9539a_1.devAddress,3,&pca9539a_1.out_data,1);
				}
				else
				{
					pca9539a_2.out_data= setval? (pca9539a_2.out_data | (1 << (channel-9))) :(pca9539a_2.out_data & (1 << (channel-9)));
					SoftI2c.writes(pca9539a_2.pins,1,pca9539a_2.devAddress,3,&pca9539a_2.out_data,1);
				}				
				printf("<OK>");
				return 1;
			}			
		}	
	}	
  else return 0;	 
}
/******************			INPUT & OUTPUT<---**************************
*/

//
