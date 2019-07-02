/*	#define LFS_BLOCK_SIZE 2048

1.	SFUN || FLASH || FLASH_ONCHIP			FLASH����
2.	FAL																FLASH�ͷ�������	������������MTD API
3.	MTD																Memory Technology Device	FLASH��ͳһ�ӿ�
4.	littlefs	
5.	DFS																Device File System ���������ļ�ϵͳ��ͳһ�����ӿ�
6.	APP

APP-->DFS API-->littlefs API--> MTD R|W -->FAL SFUD || ...

ʹ�� ls ����鿴��ǰĿ¼��Ϣ�����н��������ʾ��
msh />ls                          # ʹ�� ls ����鿴�ļ�ϵͳĿ¼��Ϣ
Directory /:                      # ���Կ����Ѿ����ڸ�Ŀ¼ /

ʹ�� echo ���������ַ��������ָ�����λ�ã����н��������ʾ��

msh />echo "hello RT-Thread!!!" hello.txt      # ���ַ���������� hello.txt �ļ�
msh />ls
Directory /:
hello.txt           18
msh />

ʹ�� cat ����鿴�ļ����ݣ����н��������ʾ��

msh />cat hello.txt                     # �鿴 hello.txt �ļ������ݲ����
hello RT-Thread!!!

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
#include <stdint.h>
#include <rtdevice.h>

#include <fal.h>
#include <dfs_fs.h>

#define DBG_SECTION_NAME               "main"
#define DBG_LEVEL                      DBG_INFO
#include <rtdbg.h>
#define FS_PARTITION_NAME              "filesystem"//"app"    

//int LittleFs_fal_flash_onchip_app(void)
int LittleFs_fal_flash_onchip_app(void)
{
    struct rt_device *mtd_dev = RT_NULL;

    fal_init();
		//need do fal probe app for fs
    mtd_dev = fal_mtd_nor_device_create(FS_PARTITION_NAME);
    if (!mtd_dev)
    {
        LOG_E("Can't create a mtd device on '%s' partition.", FS_PARTITION_NAME);
    }
    else
    {
        if (dfs_mount(FS_PARTITION_NAME, "/", "lfs", 0, 0) == 0)
        {
            LOG_I("Filesystem initialized!");
        }
        else
        {
            dfs_mkfs("lfs", FS_PARTITION_NAME);
            if (dfs_mount("filesystem", "/", "lfs", 0, 0) == 0)
            {
                LOG_I("Filesystem initialized!");
            }
            else
            {
                LOG_E("Failed to initialize filesystem!");
            }
        }
    }

//    while (1)
//    {
//       
//    }

    return RT_EOK;
}

//INIT_APP_EXPORT(LittleFs_fal_flash_onchip_app);


