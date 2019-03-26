#include "AT24CXX.h" 
#include "delay.h"

#define min(X,Y)  ( (X) > (Y) ? 	(Y) : (X) ) 

u8 dataTemp_EE[8];

//初始化IIC接口
void AT24CXX_Init(void)
{
	IIC_Init();
}
//在AT24CXX指定地址读出一个数据
u8 AT24CXX_ReadU8(u16 ReadAddr)
{				  
	IIC_DeviceReadBytes(DEVICE_TYPE,EEPROM_ADDR,ReadAddr,dataTemp_EE,1);
	return dataTemp_EE[0];
}
/**
 * Write byte
 */
void AT24CXX_WriteU8(u16 address,u8 data)
{			
	IIC_DeviceWriteBytes(DEVICE_TYPE,EEPROM_ADDR,address,&data,1);
}
//
u16 AT24CXX_ReadU16(u16 address)
{			
	IIC_DeviceReadBytes(DEVICE_TYPE,EEPROM_ADDR,address,dataTemp_EE,2);
	return *(u16*)&dataTemp_EE[0];
}
//
void AT24CXX_WriteU16(u16 address,u16 data)
{			
	IIC_DeviceWriteBytes(DEVICE_TYPE,EEPROM_ADDR,address,(u8*)&data,2);
}
/**
 * Read long
 */
u32 AT24CXX_ReadU32(u16 address)
{			
	IIC_DeviceReadBytes(DEVICE_TYPE,EEPROM_ADDR,address,dataTemp_EE,4);
	return *(u32*)&dataTemp_EE[0];
}
/**
 * Write long
 */
void AT24CXX_WriteU32(u16 address,u32 data)
{			
	IIC_DeviceWriteBytes(DEVICE_TYPE,EEPROM_ADDR,address,(u8*)&data,4);
}

/**
 * Read float
 */
f32 AT24CXX_ReadF32(u16 address)
{			
	IIC_DeviceReadBytes(DEVICE_TYPE,EEPROM_ADDR,address,dataTemp_EE,4);
	return *(float*)&dataTemp_EE[0];
}
/**
 * Write float
 */
void AT24CXX_WriteF32(u16 address,f32 data)
{			
	IIC_DeviceWriteBytes(DEVICE_TYPE,EEPROM_ADDR,address,(u8*)&data,4);
}

/**
 * Read chars
 */
void AT24CXX_Reads(u16 address,u8 *data,	u16 length)
{			
	IIC_DeviceReadBytes(DEVICE_TYPE,EEPROM_ADDR,address,data,length);
}
/**
 * Write chars
 */
void AT24CXX_Writes(u16 address,u8 *data,	u16 length)
{	
	s16 SizeLeft	 = length;						// bytes left to write
	s16 OffsetData = 0;									// current offset in data pointer
	s16 OffsetPage = 0;									// current offset in page
	s16 SizeNext 	 = 0;									// next n bytes to write	
	
	while(SizeLeft>0)									  //还有数据要写
	{	
		//页偏移地址 	
		OffsetPage	 = address % EE_PAGE_SIZE;		
		SizeNext 		 = min(min(SizeLeft, EE_PAGE_SIZE), EE_PAGE_SIZE - OffsetPage);		
		//从页偏移地址开始写入下次可写的长度
		IIC_DeviceWriteBytes(DEVICE_TYPE,EEPROM_ADDR,address,(data+OffsetData),SizeNext);
		
		SizeLeft-=SizeNext;							//剩下的字节数
		OffsetData+=SizeNext;						//下次需要写入的DATA位置
		address+=SizeNext;							//下次需要写入的ADDR位置	
	}
}
//
/********************测试函数功能，已验证OK**************************/
#ifdef EE_Test 	
typedef struct						
{
  UINT 	data4;                   
  UINT 	data8;                   
  UCHAR data9;              		
  UCHAR data10;                
	
	UCHAR data11;
	UCHAR data12;
	
	int data16; 
	int data20;
	UINT data24; 
	UINT data28; 
	
	UINT data32;	
	UINT data36;	
	
	UINT data40;
	UCHAR data41;
	UCHAR data42;
	
} TTest;

TTest TestTemp[3]=
{
	{ 
	210,   //!< VMax
  30,    //!< AMax
  3,     //!< Pulsediv
  5,     //!< Rampdiv
	
	1,		 //FlagINVDir
	1,		 //FlagINVRef
	 
	0,	 	 //XTARGET 
	0,	 	 //XACTUAL	  
	0,	   //VTARGET 
	0,		 //VACTUAL 
	 
	210,	 //HomeV1
	210,	 //HomeV2

	1,		 //HomeMode
	1,		 //isStop
	1		   //isReach
 },
	
};

#define SIZE_MOTOR_CONFIG	 42	//sizeof(TTest)=0x48=72,多处30Byte
	
u8 i,j= sizeof(TTest);
u8 datau8;
u16 datau16;
u32 datau32;
f32 dataf32;

void AT24CXX_FunctionTest(void)
{
	AT24CXX_WriteU8(0,0);
	AT24CXX_WriteU16(10,1000);
	AT24CXX_WriteU32(20,20000);
	AT24CXX_WriteF32(30,33.33);
	
	datau8=AT24CXX_ReadU8(0);
	datau16=AT24CXX_ReadU16(10);
	datau32=AT24CXX_ReadU32(20);
	dataf32=AT24CXX_ReadF32(30);
	
	for (i = 0; i < 3; i++)
	AT24CXX_Writes(i*SIZE_MOTOR_CONFIG, (UCHAR *)&TestTemp[0], SIZE_MOTOR_CONFIG);

	for (i = 0; i < 3; i++)
	AT24CXX_Reads(i*SIZE_MOTOR_CONFIG, (UCHAR *)&TestTemp[i], SIZE_MOTOR_CONFIG);
		
}

#endif


