#ifndef _MCU_USART_FIFO_H_
#define _MCU_USART_FIFO_H_

#define	UART1_FIFO_EN	0
#define	UART2_FIFO_EN	0
#define	UART3_FIFO_EN	0
#define	UART4_FIFO_EN	0
#define	UART5_FIFO_EN	0

//串口号定义
typedef enum
{
	COM1 = 0,			/* USART1  PA9, PA10 */
	COM2 = 1,			/* USART2, PA2, PA3 */
	COM3 = 2,			/* USART3, PB10, PB11 */
	COM4 = 3,			/* UART4, PC10, PC11 */
	COM5 = 4,			/* UART5, PC12, PD2 */
}COM_PORT_E;

//波特率、发送与接收缓存大小定义
#if UART1_FIFO_EN == 1
	#define UART1_BAUD				115200
	#define UART1_TX_BUF_SIZE	1*1024
	#define UART1_RX_BUF_SIZE	1*1024
#endif

#if UART2_FIFO_EN == 1
	#define UART2_BAUD				115200
	#define UART2_TX_BUF_SIZE	1*1024
	#define UART2_RX_BUF_SIZE	1*1024
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD				9600
	#define UART3_TX_BUF_SIZE	1*1024
	#define UART3_RX_BUF_SIZE	1*1024
#endif

#if UART4_FIFO_EN == 1
	#define UART4_BAUD				115200
	#define UART4_TX_BUF_SIZE	1*1024
	#define UART4_RX_BUF_SIZE	1*1024
#endif

#if UART5_FIFO_EN == 1
	#define UART5_BAUD				115200
	#define UART5_TX_BUF_SIZE	1*1024
	#define UART5_RX_BUF_SIZE	1*1024
#endif

/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;							/* STM32内部串口设备指针 */
	uint8_t *pTxBuf;									/* 发送缓冲区 */
	uint8_t *pRxBuf;									/* 接收缓冲区 */
	uint16_t usTxBufSize;							/* 发送缓冲区大小 */
	uint16_t usRxBufSize;							/* 接收缓冲区大小 */
	__IO uint16_t usTxWrite;					/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;						/* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;					/* 等待发送的数据个数 */

	__IO uint16_t usRxWrite;					/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;						/* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;					/* 还未读取的新数据个数 */

	void (*SendBefor)(void); 					/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*SendOver)(void); 					/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*ReciveNew)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
}UART_T;

void bsp_InitUart(void);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);

void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
