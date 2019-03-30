#include "bsp.h"

static KEY_T s_tBtn[KEY_COUNT];
static KEY_FIFO_T s_tKey;		

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);

#if 1	/* Ϊ������3���¼�:��K1��������, K2�������£� K1��K2ͬʱ���� */
static uint8_t IsKeyDown1(void)
{
	if ((rt_pin_read(InputPin[0]) == 0) && (rt_pin_read(InputPin[1]) != 0)
		&& (rt_pin_read(InputPin[2]) != 0))
		return 1;
	else 
		return 0;
}
static uint8_t IsKeyDown2(void)
{
	if ((rt_pin_read(InputPin[0]) != 0) && (rt_pin_read(InputPin[1]) == 0)
		&& (rt_pin_read(InputPin[2]) != 0))
		return 1;
	else 
		return 0;
}
static uint8_t IsKeyDown3(void)
{
	if ((rt_pin_read(InputPin[0]) == 0) && (rt_pin_read(InputPin[1]) == 0)
		&& (rt_pin_read(InputPin[2]) != 0))
		return 1;
	else
		return 0;
}
static uint8_t IsKeyDownK1_K2(void)	/* K1 K2��ϼ� */
{
	if ((rt_pin_read(InputPin[0]) == 0) && (rt_pin_read(InputPin[1]) == 0)
		&& (rt_pin_read(InputPin[2]) != 0))
		return 1;
	else 
		return 0;
}
static uint8_t IsKeyDownK2_K3(void)	/* K2 K3��ϼ� */
{
	if ((rt_pin_read(InputPin[0]) != 0) && (rt_pin_read(InputPin[1]) == 0)
		&& (rt_pin_read(InputPin[2]) == 0))
		return 1;
	else 
		return 0;
}
#else	
static uint8_t IsKeyDown1(void) {if ((rt_pin_read(InputPin[0])) == 0) return 1;else return 0;}
static uint8_t IsKeyDown2(void) {if ((rt_pin_read(InputPin[1])) == 0) return 1;else return 0;}
static uint8_t IsKeyDown3(void) {if ((rt_pin_read(InputPin[2])) == 0) return 1;else return 0;}

static uint8_t IsKeyDownK1_K2(void) {if (IsKeyDown1() && IsKeyDown2()) return 1;else return 0;}		/* K1 K2��ϼ� */
static uint8_t IsKeyDownK2_K3(void) {if (IsKeyDown2() && IsKeyDown3()) return 1;else return 0;}	/* K2 K3��ϼ� */
#endif

/* 5����ҡ�� */
static uint8_t IsKeyDown4(void) {if ((rt_pin_read(InputPin[3])) == 0) return 1;else return 0;}
static uint8_t IsKeyDown5(void) {if ((rt_pin_read(InputPin[4])) == 0) return 1;else return 0;}
static uint8_t IsKeyDown6(void) {if ((rt_pin_read(InputPin[5])) == 0) return 1;else return 0;}
static uint8_t IsKeyDown7(void) {if ((rt_pin_read(InputPin[6])) == 0) return 1;else return 0;}
static uint8_t IsKeyDown8(void) {if ((rt_pin_read(InputPin[7])) == 0) return 1;else return 0;}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKey
*	����˵��: ��ʼ������. �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
	bsp_InitKeyVar();			/* ��ʼ���������� */
	bsp_InitKeyHard();		/* ��ʼ������Ӳ�� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_PutKey
*	����˵��: ��1����ֵѹ�밴��FIFO��������������ģ��һ��������
*	��    ��:  _KeyCode : ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;

	if (++s_tKey.Write  >= KEY_FIFO_SIZE)
	{
		s_tKey.Write = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKey
*	����˵��: �Ӱ���FIFO��������ȡһ����ֵ��
*	��    ��:  ��
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;

	if (s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read];

		if (++s_tKey.Read >= KEY_FIFO_SIZE)
		{
			s_tKey.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKey2
*	����˵��: �Ӱ���FIFO��������ȡһ����ֵ�������Ķ�ָ�롣
*	��    ��:  ��
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
uint8_t bsp_GetKey2(void)
{
	uint8_t ret;

	if (s_tKey.Read2 == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read2];

		if (++s_tKey.Read2 >= KEY_FIFO_SIZE)
		{
			s_tKey.Read2 = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetKeyState
*	����˵��: ��ȡ������״̬
*	��    ��:  _ucKeyID : ����ID����0��ʼ
*	�� �� ֵ: 1 ��ʾ���£� 0 ��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
	return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SetKeyParam
*	����˵��: ���ð�������
*	��    �Σ�_ucKeyID : ����ID����0��ʼ
*			_LongTime : �����¼�ʱ��
*			 _RepeatSpeed : �����ٶ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;			/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;			/* �����������ٶȣ�0��ʾ��֧������ */
	s_tBtn[_ucKeyID].RepeatCount = 0;						/* ���������� */
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_ClearKey
*	����˵��: ��հ���FIFO������
*	��    �Σ���
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
void bsp_ClearKey(void)
{
	s_tKey.Read = s_tKey.Write;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKeyHard
*	����˵��: ���ð�����Ӧ��GPIO
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitKeyHard(void)
{
	uint8_t i;
	for(i=0;i<KEY_COUNT;i++)
	{
    rt_pin_mode(InputPin[i], PIN_MODE_INPUT_PULLUP); 
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitKeyVar
*	����˵��: ��ʼ����������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitKeyVar(void)
{
	uint8_t i;

	/* �԰���FIFO��дָ������ */
	s_tKey.Read = 0;
	s_tKey.Write = 0;
	s_tKey.Read2 = 0;

	/* ��ÿ�������ṹ���Ա������һ��ȱʡֵ */
	for (i = 0; i < KEY_COUNT; i++)
	{
		s_tBtn[i].LongTime = KEY_LONG_TIME;			/* ����ʱ�� 0 ��ʾ����ⳤ�����¼� */
		s_tBtn[i].Count = KEY_FILTER_TIME / 2;		/* ����������Ϊ�˲�ʱ���һ�� */
		s_tBtn[i].State = 0;							/* ����ȱʡ״̬��0Ϊδ���� */
		//s_tBtn[i].KeyCodeDown = 3 * i + 1;				/* �������µļ�ֵ���� */
		//s_tBtn[i].KeyCodeUp   = 3 * i + 2;				/* ��������ļ�ֵ���� */
		//s_tBtn[i].KeyCodeLong = 3 * i + 3;				/* �������������µļ�ֵ���� */
		s_tBtn[i].RepeatSpeed = 0;						/* �����������ٶȣ�0��ʾ��֧������ */
		s_tBtn[i].RepeatCount = 0;						/* ���������� */
	}

	/* �����Ҫ��������ĳ�������Ĳ����������ڴ˵������¸�ֵ */
	/* ���磬����ϣ������1���³���1����Զ��ط���ͬ��ֵ */
	s_tBtn[KID_K4].LongTime = 100;
	s_tBtn[KID_K4].RepeatSpeed = 5;	/* ÿ��50ms�Զ����ͼ�ֵ */

//	s_tBtn[KID_JOY_D].LongTime = 100;
//	s_tBtn[KID_JOY_D].RepeatSpeed = 5;	/* ÿ��50ms�Զ����ͼ�ֵ */

//	s_tBtn[KID_JOY_L].LongTime = 100;
//	s_tBtn[KID_JOY_L].RepeatSpeed = 5;	/* ÿ��50ms�Զ����ͼ�ֵ */

//	s_tBtn[KID_JOY_R].LongTime = 100;
//	s_tBtn[KID_JOY_R].RepeatSpeed = 5;	/* ÿ��50ms�Զ����ͼ�ֵ */

	/* �жϰ������µĺ��� */
	s_tBtn[0].IsKeyDownFunc = IsKeyDown1;
	s_tBtn[1].IsKeyDownFunc = IsKeyDown2;
	s_tBtn[2].IsKeyDownFunc = IsKeyDown3;
	s_tBtn[3].IsKeyDownFunc = IsKeyDown4;
	s_tBtn[4].IsKeyDownFunc = IsKeyDown5;
	s_tBtn[5].IsKeyDownFunc = IsKeyDown6;
	s_tBtn[6].IsKeyDownFunc = IsKeyDown7;
	s_tBtn[7].IsKeyDownFunc = IsKeyDown8;

	/* ��ϼ� */
	s_tBtn[8].IsKeyDownFunc = IsKeyDownK1_K2;
	s_tBtn[9].IsKeyDownFunc = IsKeyDownK2_K3;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DetectKey
*	����˵��: ���һ��������������״̬�����뱻�����Եĵ��á�
*	��    ��:  �����ṹ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_DetectKey(uint8_t i)
{
	KEY_T *pBtn;

	/*
		���û�г�ʼ�������������򱨴�
		if (s_tBtn[i].IsKeyDownFunc == 0)
		{
			printf("Fault : DetectButton(), s_tBtn[i].IsKeyDownFunc undefine");
		}
	*/

	pBtn = &s_tBtn[i];
	if (pBtn->IsKeyDownFunc())
	{
		if (pBtn->Count < KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count < 2 * KEY_FILTER_TIME)
		{
			pBtn->Count++;
		}
		else
		{
			if (pBtn->State == 0)
			{
				pBtn->State = 1;

				/* ���Ͱ�ť���µ���Ϣ */
				bsp_PutKey((uint8_t)(3 * i + 1));
			}

			if (pBtn->LongTime > 0)
			{
				if (pBtn->LongCount < pBtn->LongTime)
				{
					/* ���Ͱ�ť�������µ���Ϣ */
					if (++pBtn->LongCount == pBtn->LongTime)
					{
						/* ��ֵ���밴��FIFO */
						bsp_PutKey((uint8_t)(3 * i + 3));
					}
				}
				else
				{
					if (pBtn->RepeatSpeed > 0)
					{
						if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							/* ��������ÿ��10ms����1������ */
							bsp_PutKey((uint8_t)(3 * i + 1));
						}
					}
				}
			}
		}
	}
	else
	{
		if(pBtn->Count > KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count != 0)
		{
			pBtn->Count--;
		}
		else
		{
			if (pBtn->State == 1)
			{
				pBtn->State = 0;

				/* ���Ͱ�ť�������Ϣ */
				bsp_PutKey((uint8_t)(3 * i + 2));
			}
		}

		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_KeyScan
*	����˵��: ɨ�����а���������������systick�ж������Եĵ���
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_KeyScan(void)
{
	uint8_t i;

	for (i = 0; i < KEY_COUNT; i++)
	{
		bsp_DetectKey(i);
	}
}

//ʹ��timer1 �� timer2ʱû�����⣬��ʹ��timer3ʱ��timer2�Ķ�ʱʱ���Ϊ�����󣬲�Ϊ����ֵ
/*
static rt_timer_t timer3;

static void timeout3(void *parameter)
{
	//bsp_KeyScan();
	//rt_kprintf("time3 out\n" );
}
//
int timer_KeyScan_init(void)
{
    timer3 = rt_timer_create("timer3", 
                             timeout3, 
                             RT_NULL,
                             100, 			
                             RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER); 

    if (timer3 != RT_NULL) rt_timer_start(timer3);

    return 0;
}
*/

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t key_stack[ 256 ];

static struct rt_thread key_thread;

static void key_thread_entry(void *parameter)
{
	uint8_t ucKeyCode;	
	while (1)
	{
			bsp_KeyScan();
		
			rt_thread_delay(1); 
			ucKeyCode = bsp_GetKey();
			if (ucKeyCode > 0)
			{
			/* �м����� */
				switch (ucKeyCode)
				{
					case KEY_DOWN_K1:		/* K1������ */
						led_on_off(1,1);	/* ����LED1 */
						break;

					case KEY_UP_K1:		/* K1������ */
						led_on_off(1,0);	/* Ϩ��LED1 */
						break;				

					case KEY_DOWN_K2:		/* K2������ */
						led_on_off(2,1);	/* ����LED2 */
						break;

					case KEY_UP_K2:		/* K2������ */
						led_on_off(2,0);	/* Ϩ��LED2 */
						break;	

					case KEY_DOWN_K3:		/* K3������ */
						led_on_off(3,1);	/* ����LED3 */
						break;
					case KEY_UP_K3:		/* K3������ */
						led_on_off(3,0);	/* ����LED3 */
						break;
				}
			}
	}
}
//
int key_thread_init(void)
{
    rt_err_t result;

    /* init key thread */
    result = rt_thread_init(&key_thread,
                            "key",
                            key_thread_entry,
                            RT_NULL,
                            (rt_uint8_t *)&key_stack[0],
                            sizeof(key_stack),
                            20,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&key_thread);
    }
    return 0;
}

void input_read(uint8_t key_no)
{
	if(0<key_no && key_no< KEY_COUNT)
	{
		rt_kprintf("<%d>",rt_pin_read(InputPin[key_no-1]) ? 0:1 );
	}
}
//
//
void input_reads(void)
{
	char data_in[KEY_COUNT];
	uint8_t i;
	for(i=0;i<KEY_COUNT;i++)
	{
    data_in[i] = rt_pin_read(InputPin[i]) ? '0':'1';
	}
	rt_kprintf("<%.12s>",data_in);	
}
//


