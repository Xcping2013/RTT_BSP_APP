/*	�ж� �߳� ���ȼ�����

��ֵԽС�����ȼ�Խ�ߣ�0 Ϊ������ȼ�

HAL_NVIC_SetPriority(irqmap->irqno, 4, 0);
HAL_NVIC_SetPriority(irqmap->irqno, 4, 0);
HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
HAL_NVIC_SetPriority(USART2_IRQn, 6, 0);
HAL_NVIC_SetPriority(USART3_IRQn, 6, 0);
HAL_NVIC_SetPriority(TIM1_UP_IRQn, 6, 0);

#define IR_Priority_EXTI		4	 
#define IR_Priority_USART1	5
#define IR_Priority_TIM1		6
#define IR_Priority_USART3	7

#define FINSH_THREAD_PRIORITY 			18
#define RT_TIMER_THREAD_PRIO 				4	

#define RT_MAIN_THREAD_PRIORITY 		10
#define UART3_THREAD_PRIORITY				19

#define MOTORLIMIT_THREAD_PRIORITY	25

rt_thread_create
rt_thread_init

*/


/*	 �ļ���������

app_button: 
													��ʼ��������һ�ξͷ�һ��ֵ����λ�������£����븴λOK���ܽ�����������
app_gpio_dido:						IO��ʼ��+�ⲿ�жϳ�ʼ��; ��ʱ�ɼ�����״̬	;	�����жϴ���

app_motion:								����˶�����+������λ�Ĵ���	

app_motor_home:						�����ԭ��ͻ���λ����

app_serial:								Z��δֹͣʱ������3�ɿ������ݴ�ӡ�Ŀ���

app_sys_control:				  �������� EEPROM����ȡ���� SPI�������� ������ٶ����� ��ԭ�㶨ʱ�������� �����λԤ����

inc_mbtmc429:				      inc_mbtmc429�忨��Դ��CMD������

inc_mb1616dev6:				    inc_mb1616dev6�忨��Դ��CMD������

*/

/*	v2.4 20190709		�ֳ�ԭ��λ�ó����쳣����

---�����ԭ�㲻�ܱ����ֹͣ�����ϣ��������ԭ���в���Ӧ�˶�����
---�����λ��ʱ���ܽ��е���ĸ����˶�������Ҳ����ѹ������Ӱ��
---�����ԭ��͸�λ��ԭ��Ҳ���ڻ�������
---�����ԭ�㶯�����ֹ����޷���ɣ�ֻ������
		Ҫô��λ��Ҫô��λ���жϣ����������Ҫ�жϣ����鷳
//INIT_BOARD_EXPORT(rt_iwg_init);
�豸�����ʼ������ܵ�������豸��������Ȼhard fault

��Ҫ��ȫ�ֿ���	�˶�ǰ���ж��Ƿ��ǻ�ԭ�����,���ǵĻ��ȹرմ��ڴ�ӡ�ٿ��ƶ���
	closeSerial();	buttonRESETpressed	pressureAlarm	homeInfo.GoHome	homeInfo.HomeSpeed	
	
*/
/******************************************************************/
/*  v2.6 20190727  �������������Ż�	---> �����жϴ�������
1.ȥ�������ڵ���delay_ms,�ر���ɾ����ʱ���ڵ���
2.���Ĳ��ֱ�������static -->  volatile
3.�����ж����ȼ� ����3���    
4.�޸�RT_TICK_PER_SECOND---1000���߳����ȼ�			
5.����MSH�ʹ��ڻ���ռ� 
	#define RT_CONSOLEBUF_SIZE 128-->512		
	RT_SERIAL_RB_BUFSZ 64	---> 512
6.overhigh!!!  \r\n --->\n
7.HAL_SPI_TransmitReceive(&hspi2, &aTxBuffer, &aRxBuffer, 1,5000); 5000->1000

8.����SPI��ͻ������

20190730��

rt_hw_serial_register(&serial3, "uart3",RT_DEVICE_FLAG_RDONLY | RT_DEVICE_FLAG_INT_RX,uart);

uart_isr UART_FLAG_ORE�ȵ����

		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_ORE) != RESET)
		{
				__HAL_UART_CLEAR_OREFLAG(&uart->huart);
		}
		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_NE) != RESET)
		{
				__HAL_UART_CLEAR_NEFLAG(&uart->huart);
		}
		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_FE) != RESET)
		{
				__HAL_UART_CLEAR_FEFLAG(&uart->huart);
		}
		if (__HAL_UART_GET_FLAG(&(uart->huart), UART_FLAG_PE) != RESET)
		{
				__HAL_UART_CLEAR_PEFLAG(&uart->huart);
		}
*/

//
/*  v2.7.2 20190730  ѹ������	EXTI_CallBack_Fun	���Ż�   
					
INC-MBTMC429-RTT-272.hex  
���£�
 --->�������������е������˶�����,�����������ٴ���Ӧ
 --->motor reset��λ״̬���������е����������
 --->��Ҫ�ڻ�ԭ�㶯��ȥ�ɼ����λ�úͿ��Ƶ���˶�����Щ�źŻᱻ��ԭ���ϣ�����ֵ�лᴩ��һЩ�쳣����
		( Ҫô��Ҫ���ж��е���SPI,Ҫô��SPI�й�����жϣ����������ϵͳ�������Ӱ��)
     ���Է���motor reset?���ж��Ƿ��ԭ��OK	

  �Ż��ļ����� ͳһ������  stm32f103ve-512-64 �� application --- application_v2 ---std drivers

*/
//

//
