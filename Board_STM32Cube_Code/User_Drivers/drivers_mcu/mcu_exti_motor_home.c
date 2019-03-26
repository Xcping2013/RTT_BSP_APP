#include "bsp.h"
//#include "mcu_exti_motor_home.h"

uint8_t OriginPin[3]={3,4,5};  						//PE_4	PE5	PE6

void Origin_hdr_callback(void *args)
{
	rt_kprintf("Origin=%d\n",args);
	pinToggle(&PD7);
//	if(Homming.Enable[args]==TRUE)
//	{
//		if(Homming.SearchOriginDirection[args]==DIR_R)
//		{
//			if((Read429Short(IDX_VACTUAL|(args<<5)))>0);
//			else if((Read429Short(IDX_VACTUAL|(args<<5)))<0)
//			{
//				SetOrigin(args);	
//			}
//		}
//		else if(Homming.SearchOriginDirection[0]==DIR_L)
//		{
//			if((Read429Short(IDX_VACTUAL|(args<<5)))<0);
//			else if((Read429Short(IDX_VACTUAL|(args<<5)))>0)
//			{
//				SetOrigin(args);	
//			}
//		}
//	}
}


void Origin_hw_init(void)
{
	u8 i;
	
	rt_pin_mode(PD_7, PIN_MODE_OUTPUT_PP); 
	for(i=0;i<3;i++)	
	{
		rt_pin_mode(OriginPin[i], PIN_MODE_INPUT_PULLUP); 		
		rt_pin_attach_irq(OriginPin[i], PIN_IRQ_MODE_FALLING, Origin_hdr_callback,(void*)i);
	}
	rt_pin_irq_enable(OriginPin[0],PIN_IRQ_ENABLE);
	rt_pin_irq_enable(OriginPin[1],PIN_IRQ_ENABLE);
	rt_pin_irq_enable(OriginPin[2],PIN_IRQ_ENABLE);	
}


void exti_test(void)
{
	Origin_hw_init();


}
	
FINSH_FUNCTION_EXPORT(exti_test,...);
	
//void EXTI_MotorOrigin1_Init(void)
//{	
//  rt_pin_irq_enable(OriginPin[0], PIN_IRQ_ENABLE);  
//}

//void EXTI_MotorOrigin2_Init(void)
//{
//	if(act_Origin==LOW)  	rt_pin_attach_irq(OriginPin[1], PIN_IRQ_MODE_FALLING,Origin2_hdr_callback,(void*)"callback args");
//	else 	
//	{
//			rt_pin_mode(OriginPin[1], PIN_MODE_INPUT_PULLDOWN); 						
//			rt_pin_attach_irq(OriginPin[1], PIN_IRQ_MODE_RISING,Origin2_hdr_callback,(void*)"callback args");
//	}
//  rt_pin_irq_enable(OriginPin[0], PIN_IRQ_ENABLE);  
//}

//void EXTI_MotorOrigin3_Init(void)
//{
//	if(act_Origin==LOW)  	rt_pin_attach_irq(OriginPin[2], PIN_IRQ_MODE_FALLING,Origin3_hdr_callback,(void*)"callback args");
//	else 	
//	{
//			rt_pin_mode(OriginPin[2], PIN_MODE_INPUT_PULLDOWN); 						
//			rt_pin_attach_irq(OriginPin[2], PIN_IRQ_MODE_RISING,Origin3_hdr_callback,(void*)"callback args");
//	}
//  rt_pin_irq_enable(OriginPin[2], PIN_IRQ_ENABLE);  
//}

//void MotorOrigin_TI_Enable(u8 Motor)
//{
//	if(Motor==1)	EXTI_MotorOrigin1_Init();
//	if(Motor==2)	EXTI_MotorOrigin2_Init();
//	if(Motor==3)	EXTI_MotorOrigin3_Init();
//}
//void MotorOrigin_TI_Disable(u8 Motor)
//{
//	if(Motor==1)	Origin1_IRQ_Disable();
//	if(Motor==2)	EXTI_MotorOrigin23_DeInit();
//	if(Motor==3)	EXTI_MotorOrigin23_DeInit();
//}



//void Origin2_hdr_callback(void *args)
//{
//	if(Homming.Enable[1]==TRUE)
//	{
//		if(Homming.SearchOriginDirection[1]==DIR_R)
//		{
//			if((Read429Short(IDX_VACTUAL|(1<<5)))>0);
//			else if((Read429Short(IDX_VACTUAL|(1<<5)))<0)
//			{
//				SetOrigin(1);	
//			}
//		}
//		else if(Homming.SearchOriginDirection[1]==DIR_L)
//		{
//			if((Read429Short(IDX_VACTUAL|(1<<5)))<0);
//			else if((Read429Short(IDX_VACTUAL|(1<<5)))>0)
//			{
//				SetOrigin(1);	
//			}
//		}
//	}
//}

//void Origin3_hdr_callback(void *args)
//{
//	if(Homming.Enable[2]==TRUE)
//	{
//		if(Homming.SearchOriginDirection[2]==DIR_R)
//		{
//			if((Read429Short(IDX_VACTUAL|(2<<5)))>0);
//			else if((Read429Short(IDX_VACTUAL|(2<<5)))<0)
//			{
//				SetOrigin(2);	
//			}
//		}
//		else 
//		{
//			if((Read429Short(IDX_VACTUAL|(2<<5)))<0);
//			else if((Read429Short(IDX_VACTUAL|(2<<5)))>0)
//			{
//				SetOrigin(2);	
//			}
//		}
//	}	
//}

 

//void EXTI_IRQHandler_Config(void)
//{
//  GPIO_InitTypeDef GPIO_InitStruct;

//  /* GPIO Ports Clock Enable */
//  __HAL_RCC_GPIOE_CLK_ENABLE();

//  /*Configure GPIO pins : PE4 PE5 PE6 */
//  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

//  /* EXTI interrupt init*/
//  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
//  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

//  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
//  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);	
//}

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//  if (GPIO_Pin == GPIO_PIN_4)
//  {
//    rt_kprintf("Origin=1\n");

//  }
//  if (GPIO_Pin == GPIO_PIN_5)
//  {
//    rt_kprintf("Origin=2\n");

//  }
//  if (GPIO_Pin == GPIO_PIN_6)
//  {
//    rt_kprintf("Origin=3\n");
//  }
//}


//void Origin_hdr_callback(void *args)
//{
//	rt_kprintf("Origin=%d\n",args);
//	pinToggle(&PD7);
////	if(Homming.Enable[args]==TRUE)
////	{
////		if(Homming.SearchOriginDirection[args]==DIR_R)
////		{
////			if((Read429Short(IDX_VACTUAL|(args<<5)))>0);
////			else if((Read429Short(IDX_VACTUAL|(args<<5)))<0)
////			{
////				SetOrigin(args);	
////			}
////		}
////		else if(Homming.SearchOriginDirection[0]==DIR_L)
////		{
////			if((Read429Short(IDX_VACTUAL|(args<<5)))<0);
////			else if((Read429Short(IDX_VACTUAL|(args<<5)))>0)
////			{
////				SetOrigin(args);	
////			}
////		}
////	}
//}



//void exti_test(void)
//{
//	EXTI_IRQHandler_Config();
////	rt_pin_irq_enable(OriginPin[0],PIN_IRQ_ENABLE);
////	rt_pin_irq_enable(OriginPin[1],PIN_IRQ_ENABLE);
////	rt_pin_irq_enable(OriginPin[2],PIN_IRQ_ENABLE);

//}