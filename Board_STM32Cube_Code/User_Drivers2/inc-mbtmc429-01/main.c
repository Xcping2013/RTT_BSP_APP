/*
*********************************************************************************************************
*
*	ģ������ : ���������
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : ����ͨ�����ӣ���PC�������ն�������н���
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2015-08-30 armfly  �׷�
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"				/* �ײ�Ӳ������ */

/* ���������������̷������� */
#define EXAMPLE_NAME	"V4-003_���ں�PC��ͨ�ţ������жϡ�FIFO���ƣ�"
#define EXAMPLE_DATE	"2015-08-30"
#define DEMO_VER		"1.0"

static void PrintfLogo(void);
static void PrintfHelp(void);

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t ucKeyCode;	
	int16_t count = 0;
	uint8_t fRefresh = 0;
	uint8_t read;
	
	/*
		ST�̼����е������ļ��Ѿ�ִ���� SystemInit() �������ú����� system_stm32f4xx.c �ļ�����Ҫ������
	����CPUϵͳ��ʱ�ӣ��ڲ�Flash����ʱ������FSMC�����ⲿSRAM
	*/
	bsp_Init();		/* Ӳ����ʼ�� */
	
	PrintfLogo();	/* ��ӡ�������ƺͰ汾����Ϣ */
	PrintfHelp();	/* ��ӡ������ʾ */
	
	fRefresh = 1;
	/* �������ѭ�� */
	while (1)
	{
		bsp_Idle();		/* CPU����ʱִ�еĺ������� bsp.c */
	
		/* ҡ�����Ҽ������¼�������LED���� */
		if (fRefresh)
		{
			fRefresh = 0;

			#if 0	/* ������ printf��ӡ���ݵ����� */
				printf("count = %6d \r", count);
			#else	/* Ҳ������ sprintf�������һ��buf��Ȼ���ڷ��͵����� */
			{
				char buf[64];
								
				sprintf(buf, "count = %6d \r", count);
				
				comSendBuf(COM1, (uint8_t *)buf, strlen(buf));
			}
			#endif
		}
		
		if (comGetChar(COM1, &read))
		{
			switch (read)
			{
				case '1':
					bsp_LedToggle(1);
					break;

				case '2':
					bsp_LedToggle(2);
					break;

				case '3':
					bsp_LedToggle(3);
					break;

				case '4':
					bsp_LedToggle(4);
					break;	
			}

		}
		
		/* �������¼� */
		ucKeyCode = bsp_GetKey();
		if (ucKeyCode > 0)
		{
			/* �м����� */
			switch (ucKeyCode)
			{
				case JOY_DOWN_L:		/* ҡ��LEFT������, ����LED��ʾ���� */
					count-=10;
					fRefresh = 1;
					break;		
				
				case JOY_DOWN_R:		/* ҡ��RIGHT������ */
					count+=10;		
					fRefresh = 1;
					break;

				case JOY_DOWN_U:		/* ҡ��UP������ */
					count++;		
					fRefresh = 1;
					break;				
				
				case JOY_DOWN_D:		/* ҡ��DOWN������ */
					count--;
					fRefresh = 1;
					break;

				case JOY_DOWN_OK:		/* ҡ��OK������ */
					break;		
				
				default:
					break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: PrintfHelp
*	����˵��: ��ӡ������ʾ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfHelp(void)
{
	printf("������ʾ:\r\n");
	printf("1. ҡ�˿���count����ֵ\r\n");
	printf("     �ϼ� = +1\r\n");
	printf("     �¼� = -1\r\n");		
	printf("     ��� = -10\r\n");
	printf("     �Ҽ� = +10\r\n");
	printf("2. PC����������1-4���ƿ������ϵ�LEDָʾ��\r\n");
}

/*
*********************************************************************************************************
*	�� �� ��: PrintfLogo
*	����˵��: ��ӡ�������ƺ����̷�������, ���ϴ����ߺ󣬴�PC���ĳ����ն�������Թ۲���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	printf("*************************************************************\n\r");
	printf("* ��������   : %s\r\n", EXAMPLE_NAME);	/* ��ӡ�������� */
	printf("* ���̰汾   : %s\r\n", DEMO_VER);		/* ��ӡ���̰汾 */
	printf("* ��������   : %s\r\n", EXAMPLE_DATE);	/* ��ӡ�������� */

	/* ��ӡST�̼���汾���궨���� stm32f4xx.h �ļ� */
	printf("* �̼���汾 : %d.%d.%d\r\n", __STM32F10X_STDPERIPH_VERSION_MAIN,
			__STM32F10X_STDPERIPH_VERSION_SUB1,__STM32F10X_STDPERIPH_VERSION_SUB2);

	/* ��ӡ CMSIS �汾. �궨���� core_cm4.h �ļ� */
	printf("* CMSIS�汾  : %X.%02X\r\n", __CM3_CMSIS_VERSION_MAIN, __CM3_CMSIS_VERSION_SUB);

	printf("* \n\r");	/* ��ӡһ�пո� */
	printf("* QQ    : 1295744630 \r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* Copyright www.armfly.com ����������\r\n");
	printf("*************************************************************\n\r");
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
