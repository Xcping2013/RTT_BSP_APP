#include "bsp.h"

static KEY_T s_tBtn[KEY_COUNT];
static KEY_FIFO_T s_tKey;		

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);

#if 1	/* 为了区分3个事件:　K1单独按下, K2单独按下， K1和K2同时按下 */
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
static uint8_t IsKeyDownK1_K2(void)	/* K1 K2组合键 */
{
	if ((rt_pin_read(InputPin[0]) == 0) && (rt_pin_read(InputPin[1]) == 0)
		&& (rt_pin_read(InputPin[2]) != 0))
		return 1;
	else 
		return 0;
}
static uint8_t IsKeyDownK2_K3(void)	/* K2 K3组合键 */
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

static uint8_t IsKeyDownK1_K2(void) {if (IsKeyDown1() && IsKeyDown2()) return 1;else return 0;}		/* K1 K2组合键 */
static uint8_t IsKeyDownK2_K3(void) {if (IsKeyDown2() && IsKeyDown3()) return 1;else return 0;}	/* K2 K3组合键 */
#endif

/* 5方向摇杆 */
static uint8_t IsKeyDown4(void) {if ((rt_pin_read(InputPin[3])) == 0) return 1;else return 0;}
static uint8_t IsKeyDown5(void) {if ((rt_pin_read(InputPin[4])) == 0) return 1;else return 0;}
static uint8_t IsKeyDown6(void) {if ((rt_pin_read(InputPin[5])) == 0) return 1;else return 0;}
static uint8_t IsKeyDown7(void) {if ((rt_pin_read(InputPin[6])) == 0) return 1;else return 0;}
static uint8_t IsKeyDown8(void) {if ((rt_pin_read(InputPin[7])) == 0) return 1;else return 0;}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitKey
*	功能说明: 初始化按键. 该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
	bsp_InitKeyVar();			/* 初始化按键变量 */
	bsp_InitKeyHard();		/* 初始化按键硬件 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_PutKey
*	功能说明: 将1个键值压入按键FIFO缓冲区。可用于模拟一个按键。
*	形    参:  _KeyCode : 按键代码
*	返 回 值: 无
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
*	函 数 名: bsp_GetKey
*	功能说明: 从按键FIFO缓冲区读取一个键值。
*	形    参:  无
*	返 回 值: 按键代码
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
*	函 数 名: bsp_GetKey2
*	功能说明: 从按键FIFO缓冲区读取一个键值。独立的读指针。
*	形    参:  无
*	返 回 值: 按键代码
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
*	函 数 名: bsp_GetKeyState
*	功能说明: 读取按键的状态
*	形    参:  _ucKeyID : 按键ID，从0开始
*	返 回 值: 1 表示按下， 0 表示未按下
*********************************************************************************************************
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
	return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SetKeyParam
*	功能说明: 设置按键参数
*	形    参：_ucKeyID : 按键ID，从0开始
*			_LongTime : 长按事件时间
*			 _RepeatSpeed : 连发速度
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;			/* 长按时间 0 表示不检测长按键事件 */
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;			/* 按键连发的速度，0表示不支持连发 */
	s_tBtn[_ucKeyID].RepeatCount = 0;						/* 连发计数器 */
}


/*
*********************************************************************************************************
*	函 数 名: bsp_ClearKey
*	功能说明: 清空按键FIFO缓冲区
*	形    参：无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_ClearKey(void)
{
	s_tKey.Read = s_tKey.Write;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitKeyHard
*	功能说明: 配置按键对应的GPIO
*	形    参:  无
*	返 回 值: 无
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
*	函 数 名: bsp_InitKeyVar
*	功能说明: 初始化按键变量
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitKeyVar(void)
{
	uint8_t i;

	/* 对按键FIFO读写指针清零 */
	s_tKey.Read = 0;
	s_tKey.Write = 0;
	s_tKey.Read2 = 0;

	/* 给每个按键结构体成员变量赋一组缺省值 */
	for (i = 0; i < KEY_COUNT; i++)
	{
		s_tBtn[i].LongTime = KEY_LONG_TIME;			/* 长按时间 0 表示不检测长按键事件 */
		s_tBtn[i].Count = KEY_FILTER_TIME / 2;		/* 计数器设置为滤波时间的一半 */
		s_tBtn[i].State = 0;							/* 按键缺省状态，0为未按下 */
		//s_tBtn[i].KeyCodeDown = 3 * i + 1;				/* 按键按下的键值代码 */
		//s_tBtn[i].KeyCodeUp   = 3 * i + 2;				/* 按键弹起的键值代码 */
		//s_tBtn[i].KeyCodeLong = 3 * i + 3;				/* 按键被持续按下的键值代码 */
		s_tBtn[i].RepeatSpeed = 0;						/* 按键连发的速度，0表示不支持连发 */
		s_tBtn[i].RepeatCount = 0;						/* 连发计数器 */
	}

	/* 如果需要单独更改某个按键的参数，可以在此单独重新赋值 */
	/* 比如，我们希望按键1按下超过1秒后，自动重发相同键值 */
	s_tBtn[KID_K4].LongTime = 100;
	s_tBtn[KID_K4].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */

//	s_tBtn[KID_JOY_D].LongTime = 100;
//	s_tBtn[KID_JOY_D].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */

//	s_tBtn[KID_JOY_L].LongTime = 100;
//	s_tBtn[KID_JOY_L].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */

//	s_tBtn[KID_JOY_R].LongTime = 100;
//	s_tBtn[KID_JOY_R].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */

	/* 判断按键按下的函数 */
	s_tBtn[0].IsKeyDownFunc = IsKeyDown1;
	s_tBtn[1].IsKeyDownFunc = IsKeyDown2;
	s_tBtn[2].IsKeyDownFunc = IsKeyDown3;
	s_tBtn[3].IsKeyDownFunc = IsKeyDown4;
	s_tBtn[4].IsKeyDownFunc = IsKeyDown5;
	s_tBtn[5].IsKeyDownFunc = IsKeyDown6;
	s_tBtn[6].IsKeyDownFunc = IsKeyDown7;
	s_tBtn[7].IsKeyDownFunc = IsKeyDown8;

	/* 组合键 */
	s_tBtn[8].IsKeyDownFunc = IsKeyDownK1_K2;
	s_tBtn[9].IsKeyDownFunc = IsKeyDownK2_K3;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_DetectKey
*	功能说明: 检测一个按键。非阻塞状态，必须被周期性的调用。
*	形    参:  按键结构变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_DetectKey(uint8_t i)
{
	KEY_T *pBtn;

	/*
		如果没有初始化按键函数，则报错
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

				/* 发送按钮按下的消息 */
				bsp_PutKey((uint8_t)(3 * i + 1));
			}

			if (pBtn->LongTime > 0)
			{
				if (pBtn->LongCount < pBtn->LongTime)
				{
					/* 发送按钮持续按下的消息 */
					if (++pBtn->LongCount == pBtn->LongTime)
					{
						/* 键值放入按键FIFO */
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
							/* 常按键后，每隔10ms发送1个按键 */
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

				/* 发送按钮弹起的消息 */
				bsp_PutKey((uint8_t)(3 * i + 2));
			}
		}

		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_KeyScan
*	功能说明: 扫描所有按键。非阻塞，被systick中断周期性的调用
*	形    参:  无
*	返 回 值: 无
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

//使用timer1 和 timer2时没有问题，当使用timer3时，timer2的定时时间变为超级大，不为设置值
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
			/* 有键按下 */
				switch (ucKeyCode)
				{
					case KEY_DOWN_K1:		/* K1键按下 */
						led_on_off(1,1);	/* 点亮LED1 */
						break;

					case KEY_UP_K1:		/* K1键弹起 */
						led_on_off(1,0);	/* 熄灭LED1 */
						break;				

					case KEY_DOWN_K2:		/* K2键按下 */
						led_on_off(2,1);	/* 点亮LED2 */
						break;

					case KEY_UP_K2:		/* K2键弹起 */
						led_on_off(2,0);	/* 熄灭LED2 */
						break;	

					case KEY_DOWN_K3:		/* K3键按下 */
						led_on_off(3,1);	/* 点亮LED3 */
						break;
					case KEY_UP_K3:		/* K3键按下 */
						led_on_off(3,0);	/* 点亮LED3 */
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


