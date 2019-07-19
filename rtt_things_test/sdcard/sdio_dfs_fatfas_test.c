/*	
1.bsp\stm32\...  --> env: scons --dist

2.board\CubeMX_Config\sdio set\generate

3.board\Kconfig :config BSP_USING_WDT
    config BSP_USING_SDIO
            bool "Enable SDIO BUS"
            select RT_USING_SDIO
            default    n
4.env: menuconfig    Enable SDIO BUS
5.DFS elm-chan fatfs
6.scons --target=mdk5
7.asm-download-debug

*/

/*	DFS Ӧ��ʾ��			FinSH ����


FinSH ����		����
ls						��ʾ�ļ���Ŀ¼����Ϣ
cd						����ָ��Ŀ¼
cp						�����ļ�
rm						ɾ���ļ���Ŀ¼
mv						���ļ��ƶ�λ�û����
echo					��ָ������д��ָ���ļ������ļ�����ʱ����д����ļ������ļ�������ʱ���´���һ���ļ���д��
cat						չʾ�ļ�������
pwd						��ӡ����ǰĿ¼��ַ
mkdir					�����ļ���
mkfs					��ʽ���ļ�ϵͳ							mkfs sd0 		mkfs -t elm sd0	ʹ�� -t ����ָ�����ļ�ϵͳ����Ϊ elm-FAT �ļ�ϵͳ

mkdir rt-thread            								 # ���� rt-thread �ļ���
echo "hello rt-thread!!!"              	   # ���ַ����������׼���
echo "hello rt-thread!!!" hello.txt     	 # ���ַ���������� hello.txt �ļ�
cat hello.txt                    					 # �鿴 hello.txt �ļ������ݲ����
rm rt-thread                      				 # ɾ�� rt-thread �ļ���
rm hello.txt                   					   # ɾ�� hello.txt �ļ�
msh /dir_test>cd ..                        # �л����ϼ��ļ���

open	write ||read	close
rename
stat
mkdir
opendir	readdir	closedir
opendir	readdir	telldir	seekdir	telldir	readdir	closedir

*/
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <dfs_posix.h>

int sdio_fatfs_init(void)
{
   struct rt_device *sd_dev = RT_NULL;

		while(1)
		{
			
			sd_dev = rt_device_find("sd0");
			
			if (sd_dev!=RT_NULL)
			{
        if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
        {
            rt_kprintf("sd card mount to / success!\n");
        }
        else
        {
            rt_kprintf("sd card mount to / failed!\n");
        }
				break;
			}
			rt_thread_delay(50);
		}
    return RT_EOK;
}

INIT_APP_EXPORT(sdio_fatfs_init);


