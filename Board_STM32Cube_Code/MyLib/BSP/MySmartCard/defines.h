/**
  ******************************************************************************
  * @file    Defines.h
  * @author  XCP
  * @version V1.0
  * @date    2016-2-25
  * @brief   Header for Defines.c module
	概要：
				1.变量类型重定义	2.FALSE TRUE	NULL	BIT		3.位带操作、软件复位、中断失能
				4.数据结构
				
  ******************************************************************************
**/
#ifndef __DEFINE_H
#define __DEFINE_H

//#include "stm32f1xx_hal.h"
//#include "stdarg.h"
#include "stdio.h"
/*********************1.变量类型重定义*******************************************/
typedef unsigned char 	u8; 
typedef signed char     S8_T;
typedef unsigned short  u16;
typedef short          	s16;
typedef short          	S16_T;
typedef unsigned long   u32;

typedef long           	S32_T;

typedef unsigned char 	UCHAR;                   //!< 8 bits unsigned
typedef unsigned short  USHORT;  	               //!< 16 bits unsigned
typedef unsigned int 		UINT;                    //!< 32 bits unsigned

typedef unsigned char   			U8_T;
typedef unsigned short  			U16_T;
typedef unsigned long   			U32_T;
typedef unsigned long long 		U64_T;

/* Float Types                                                               */
typedef float		F32_T;
typedef double	F64_T;

/* Boolean types declared as U8_T, as enums are generated as 16 bit          */
typedef U8_T	B_T;

/*********************2.状态宏定义*******************************************/
/* Definitions for the two members of the Boolean type                       */
#ifndef FALSE
#define FALSE	((B_T) 0)
#endif

#ifndef TRUE
#define TRUE	((B_T) 1)
#endif

/* NULL definition */
#ifndef NULL
#define NULL  0
#endif

/*********************3.	32位对应位宏定义*******************************************/
#define   BIT0        0x00000001
#define   BIT1        0x00000002
#define   BIT2        0x00000004
#define   BIT3        0x00000008
#define   BIT4        0x00000010
#define   BIT5        0x00000020
#define   BIT6        0x00000040
#define   BIT7        0x00000080
#define   BIT8        0x00000100
#define   BIT9        0x00000200
#define   BIT10       0x00000400
#define   BIT11       0x00000800
#define   BIT12       0x00001000
#define   BIT13       0x00002000
#define   BIT14       0x00004000
#define   BIT15       0x00008000
#define   BIT16       0x00010000
#define   BIT17       0x00020000
#define   BIT18       0x00040000
#define   BIT19       0x00080000
#define   BIT20       0x00100000
#define   BIT21       0x00200000
#define   BIT22       0x00400000
#define   BIT23       0x00800000
#define   BIT24       0x01000000
#define   BIT25       0x02000000
#define   BIT26       0x04000000
#define   BIT27       0x08000000
#define   BIT28       0x10000000
#define   BIT29       0x20000000
#define   BIT30       0x40000000
#define   BIT31       0x80000000

//位带操作
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  

//常量表格
//const float y0y1_mc_table[11][2]={};
//16位数据------8位*2
typedef union  
{
	int            sint;
  unsigned int   usint;
	unsigned char  ubyte;
  u8  ch[2];
}IntChar;
//32位数据------8位*4
typedef  union
{
	float fval;
	long  lval;
	int   				word;
	unsigned int 	uword;
	unsigned char ubyte;

	struct{
				 u8 char1;
				 u8 char2;
				 u8 char3;
				 u8 char4;
	}bytes;
}WordByte;

//参数成员
typedef struct
{
	unsigned char name[4];      //显示参数
	unsigned int  address;      //数据地址
	unsigned char leng;         //数据长度
	int           upper;        //数据下限
	int           lower;        //数据上限  
	int           vaule;        //设定值
	unsigned char sign;         //正负
}DatesItem;

typedef union
{
	float f32;  
	//long	i32;
	u8 		Byte[4];   	
	
}Data_4Bytes;


#endif

