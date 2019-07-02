/*	FAL (Flash Abstraction Layer) Flash �����
	�� Flash ������ Flash �ķ������й��������ĳ���㣬���ϲ�ͳһ�� Flash �� ���������� API

1. ���� Flash �豸 �� ������д�����Ȳ�������

	 fal_flash_stm32f2_port.c || drv_flash_f1.c	|| fal_flash_sfud_port.c
	 
	 const struct fal_flash_dev stm32f2_onchip_flash = { "stm32_onchip", 0x08000000, 1024*1024, 128*1024, {init, read, write, erase} };
	 
	 ���û�� init ��ʼ�����̣������ÿ�
	 
2. ���� Flash �豸��	

	fal_cfg.h	-->	FAL_FLASH_DEV_TABLE

3. ���÷��� : ������	Flash�豸��	ƫ�Ƶ�ַ	��С	˵��

	drv_flash_f1.c	fal_cfg.h		fal.c
		
stm32f2_onchip_flash	---		stm32f_onchip_flash
														BSP_USING_ON_CHIP_FLASH	STM32_FLASH_START_ADRESS...	LOG_E	LOG_D
stm32_onchip					---		onchip_flash
		
fal_init();
		
*/

#include <rtthread.h>
#include <stdint.h>
#include <rtdevice.h>

int fal_flash_onchip_app(void)
{
	/*
drv_flash_f1.c	onchip_flash	��������     easyFlash    FAL	 �ļ�ϵͳ
										spi_flash              easyFlash    FAL  �ļ�ϵͳ	
	*/
    /* user app entry */
    return 0;
}

int fal_flash_sfud_app(void)
{
    /* user app entry */
    return 0;
}



