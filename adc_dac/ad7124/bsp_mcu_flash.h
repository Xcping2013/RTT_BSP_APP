/*
STM32 ��FLASHģ���ɣ����洢�� ��Ϣ�� ����洢���ӿڼĴ��� �� 3 ������ɡ�

���洢��:�ò���������Ŵ�������ݳ������� const ���͵����ݣ�
  ���ڴ�������Ʒ���䱻����Ϊ 256 ҳ��ÿҳ 2K �ֽڡ�ע�⣬С��������������Ʒ��ÿҳֻ�� 1K �ֽڡ�
  ���洢������ʼ��ַ���� 0X08000000�� B0�� B1 ���� GND ��ʱ�򣬾��Ǵ� 0X08000000��ʼ���д����

��Ϣ��:�ò��ַ�Ϊ 2 ��С���֣���������������룬�������洢 ST �Դ��������������ڴ������ش���
	�� B0 �� V3.3�� B1 �� GND ��ʱ�����еľ����ⲿ�ִ��롣�û�ѡ���ֽڣ���һ����������д�������������ȹ���

����洢���ӿڼĴ���:�ò������ڿ��������д�ȣ�����������ģ��Ŀ��ƻ���

��������ģ�������ͨ�õ�ַ�ռ�ֱ��Ѱַ���κ� 32 λ���ݵĶ��������ܷ�������ģ������ݲ��õ���Ӧ�����ݡ�

STM32 �����������Ϊ���֣�ҳ��������Ƭ����

*/

#ifndef __BSP_MCU_DELAY_H
#define __BSP_MCU_DELAY_H 

#include "bsp_defines.h"

//STM32F103VET:	V=100pins;E=512 Kbytes of flash memory

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Base address of the Flash sectors */
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 2 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000800) /* Base @ of Page 1, 2 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08001000) /* Base @ of Page 2, 2 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x08001800) /* Base @ of Page 3, 2 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x08002000) /* Base @ of Page 4, 2 Kbytes */
#define ADDR_FLASH_PAGE_5    ((uint32_t)0x08002800)  /* Base @ of Page 5, 2 Kbytes */
#define ADDR_FLASH_PAGE_6    ((uint32_t)0x08003000) /* Base @ of Page 12, 2 Kbytes */

#define ADDR_FLASH_PAGE_127   ((uint32_t)0x0803F800) /* Base @ of Page 127, 2 Kbytes */

#define ADDR_FLASH_PAGE_255   ((uint32_t)0x0807F000) /* Base @ of Page 127, 2 Kbytes */


/* Exported functions ------------------------------------------------------- */

#endif
