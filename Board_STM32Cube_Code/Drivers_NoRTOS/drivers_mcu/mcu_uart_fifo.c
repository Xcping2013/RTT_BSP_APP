#include "bsp.h"

/*
//rt_hw_usart_init			  --->    		rt_hw_serial_register

	//stm32_uart		 					--->				UART_HandleTypeDef IRQn_Type
	//stm32_configure					--->				HAL_UART_Init --- MX_USART_UART_Init --- HAL_UART_MspInit --- HAL_UART_MspDeInit
	//stm32_control	 					--->				__HAL_UART_ENABLE_IT
	//stm32_putc							--->				uart->huart.Instance->DR = c;
	//stm32_getc							--->				ch = uart->huart.Instance->DR & 0xff;
	//
	//stm32_uart_ops					--->				stm32_configure ---	control	--- putc ---	getc

//serial_ops
	//rt_serial_init					--->				serial->ops->configure
	
//USARTx_IRQHandler			--->    		uart_isr	--->	rt_hw_serial_isr	__HAL_UART_CLEAR_FLAG

	//rx_length	--->		dev->rx_indicate = rx_ind --->	rt_device_set_rx_indicate	--- finsh_rx_ind
	//serial->serial_rx			--->					*buffer --- put_index --- get_index --- is_full			//ȡ�����±�=�������±꣬˵������ȡ�꣬ȡ�±�Ҳѭ��
	//rt_hw_serial_isr			--->					rx_fifo->buffer[rx_fifo->put_index]	--- rx_fifo->put_index += 1;				//�Ѵ����ַ�������ջ�����
	//																		if (rx_fifo->put_index >= serial->config.bufsz) rx_fifo->put_index = 0;	//���ճ����棬��������
	//																																		---> RT_SERIAL_RB_BUFSZ	
		//struct rt_serial_rx_fifo
		//{
		//    rt_uint8_t *buffer;

		//    rt_uint16_t put_index, get_index;

		//    rt_bool_t is_full;
		//};
*/


