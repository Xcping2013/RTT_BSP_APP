/*
ef_fal_port.c			#define FAL_EF_PART_NAME                "parm"

fal_init();

easyflash_init();

ENV: Environment variables�� 																									 	�Ƿ�ʹ�ܻ�����������
Auto update ENV to latest default when current ENV version number is changed.��	�Ƿ����û��������Զ����¹��ܡ�����������ܺ󣬻�������������汾�ŷ����仯ʱ�Զ����¡�
Setting current ENV version number : 																						��ǰ���������汾��
LOG: Save logs on flash��																												��־���ܣ����Խ���־˳�򱣴��� Flash �С���������� EasyLogger ��ɲ�Ʒ��־�ĵ���洢��
IAP: In Application Programming��																								IAP �����������ܣ��������ṩһЩ IAP �����ﳣ�õ� API ��
Erase minimum granularity��																											��������С���ȣ�һ�� SPI Flash ͨ��Ϊ 4KB��STM32F4 Ƭ�� Flash ͨ��Ϊ 128KB��
Write minimum granularity��																											д���ݵ���С���ȣ�һ�� SPI Flash ͨ��Ϊ 1bit��STM32F4 Ƭ�� Flash ͨ��Ϊ 8bit���������ѡ�
Start addr on flash or partition��																							EasyFlash �������洢������� Flash ���� ���� ��ƫ�Ƶ�ַ������ֲ���������
Enable debug log output��																												�Ƿ�ʹ�ܵ�����־����������󽫻ῴ�����������־��Ϣ��
*/

/*
http://packages.rt-thread.org/itemDetail.html?package=EasyFlash
ENV ���ٱ����Ʒ����	ENV: Environment variables��
ROM: 6K bytes     RAM: 0.1K bytes

fal+easyflash
E:\rt-thread\bsp\stm32f10x-HAL\packages\fal-latest\samples\porting\fa_cfg.h
E:\rt-thread\bsp\stm32f10x-HAL\packages\EasyFlash-latest\ports\ef_fal_port.c

https://git.oschina.net/Armink/EasyFlash/repository/archive?ref=master

https://coding.net/u/armink/p/EasyFlash/git/archive/master

used:

EasyFlash-master\easyflash\port\ef_port.c
EasyFlash-master\demo\env\stm32f10x\rtt\components\easyflash\port\ef_port.c

reconfig.h				#define FAL_PART_TABLE_FLASH_DEV_NAME 	"onchip_flash"					

drv_flash_f1.c		const struct fal_flash_dev stm32_onchip_flash = { "onchip_flash", STM32_FLASH_START_ADRESS, STM32_FLASH_SIZE, FLASH_PAGE_SIZE, {NULL, fal_flash_read, fal_flash_write, fal_flash_erase} };
fal_cfg.h					FAL_PART_TABLE


*/
#include <rtthread.h>
#include <stdint.h>
#include <rtdevice.h>

int fal_easyflash_onchip_app(void)
{
    /* user app entry */
//	fal_init();

//	easyflash_init();
    return 0;
}

int fal_easyflash_sfud_app(void)
{
    /* user app entry */
    return 0;
}



