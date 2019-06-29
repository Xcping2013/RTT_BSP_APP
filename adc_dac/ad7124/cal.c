
#include "cal.h"
#include "string.h"
#include "dmm.h"
#include <stdlib.h>
#include "ad7124.h"
#include "ad7124_regs.h"

#define FLASH_USER_START_ADDR    ((uint32_t)0x0803F800)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_127 + FLASH_PAGE_SIZE   /* End @ of user Flash area */

#define FLASH_ADDR ( 0x08000000 + 256 * 1024 )	//page128

uint32_t PAGEError = 0;

static FLASH_EraseInitTypeDef EraseInitStruct;

float KTable[ DMM_FUNC_COUNT ];

float CTable[ DMM_FUNC_COUNT ];

float Cal_GetK( int _Func )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		return 	KTable[ _Func ];	
	}
	return 1.25f;
} 

void Cal_SetK( int _Func , float _K )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		KTable[ _Func ] = _K ;
	}
}

float Cal_GetC( int _Func )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		return 	CTable[ _Func ];	
	}
	return 1.25f;
} 

void Cal_SetC( int _Func , float _C )
{
	if( _Func > DMM_FUNC_UNKNOW && _Func < DMM_FUNC_COUNT )
	{
		CTable[ _Func ] = _C ;
	}
}

void Cal_IdentifyRam( void )
{
	unsigned int i= 0;
	for( ; i < DMM_FUNC_COUNT ; ++i )
	{
		//KTable[ i ] = 1.25f;	
		KTable[ i ] = 1.0f;	
		CTable[ i ] = 0.0f;	
	}	
}

// 将RAM中的校准数据保存到Flash
void Cal_RamToFlash( void )
{
	unsigned int i = 0;
	unsigned int Addr = FLASH_ADDR;
	HAL_FLASH_Unlock();
	
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
	EraseInitStruct.NbPages=1;
  //EraseInitStruct.NbPages     = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

	HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
	
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Addr, 0xA5A1A2A4);

	Addr += 4;
   for( i = 0 ; i < sizeof( KTable ) / sizeof(KTable[0]) ; ++i )
	{
       	HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, Addr , *(((u32*)(&KTable))+i));
		Addr += 4;
	}
    for( i = 0 ; i < sizeof( CTable ) / sizeof(CTable[0]) ; ++i )
	{
       	HAL_FLASH_Program( FLASH_TYPEPROGRAM_WORD, Addr , *(((u32*)(&CTable))+i));
		Addr += 4;
	}
	HAL_FLASH_Lock();
}
// 将flash 中的数据加载到
void Cal_FlashToRam()
{
	unsigned int i = 0;
	float* pFlash = (float*)(FLASH_ADDR);
    HAL_FLASH_Unlock();
	if( *(u32*)pFlash == 0xA5A1A2A4 )
	{
    	++pFlash;
		for( i = 0 ; i < sizeof( KTable ) / sizeof(KTable[0]) ; ++i )
		{
	       	*(((float*)(&KTable))+i) = *pFlash;
			++pFlash;
		}
		for( i = 0 ; i < sizeof( CTable ) / sizeof(CTable[0]) ; ++i )
		{
	       	*(((float*)(&CTable))+i) = *pFlash;
			++pFlash;
		}
	}
	else
	{
		Cal_IdentifyRam();
	}
    HAL_FLASH_Lock();
}
 
#if 0

static float KRead[ DMM_FUNC_COUNT ];

static float CRead[ DMM_FUNC_COUNT ];

void flash_data_write_test(void)
{
	unsigned int i= 0;
	for( ; i < DMM_FUNC_COUNT ; ++i )
	{
		KTable[ i ] = 12.34f;	
		CTable[ i ] = 43.21f;	
	}
	Cal_RamToFlash();
	
}
void flash_data_read_test(void)
{
	unsigned int i = 0;
	
	uint8_t falsh_value_str[10];

	float* pFlash = (float*)(FLASH_ADDR);
   HAL_FLASH_Unlock();
	if( *(u32*)pFlash == 0xA5A1A2A4 )
	{
    	++pFlash;
		for( i = 0 ; i < sizeof( KRead ) / sizeof(KRead[0]) ; ++i )
		{
	       	*(((float*)(&KRead))+i) = *pFlash;
			++pFlash;
		}
		for( i = 0 ; i < sizeof( CRead ) / sizeof(CRead[0]) ; ++i )
		{
	       	*(((float*)(&CRead))+i) = *pFlash;
			++pFlash;
		}
	}
	HAL_FLASH_Lock();
	for( i=0; i < DMM_FUNC_COUNT ; ++i )
	{
		sprintf(falsh_value_str,"%lf",KRead[i]);							
		AD7124_TRACE("KRead[%d]=%s\n",i,falsh_value_str);
		sprintf(falsh_value_str,"%lf",CRead[i]);							
		AD7124_TRACE("CRead[%d]=%s\n",i,falsh_value_str);
	}
}
MSH_CMD_EXPORT_ALIAS(flash_data_read_test, flash_r,...);
MSH_CMD_EXPORT_ALIAS(flash_data_write_test, flash_w,...);
#endif

