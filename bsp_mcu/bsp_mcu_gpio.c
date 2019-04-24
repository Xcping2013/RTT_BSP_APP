//#include "bsp_include.h"	
//#include "app_include.h"
#include "bsp_mcu_delay.h"
#include "bsp_mcu_gpio.h"


#ifndef	USING_RTT

	#define	STM32F10X_PIN_NUMBERS 100
	//#define USING_STM32F030F4	
	
	#define __STM32_PIN(index, gpio, gpio_index) (gpio | gpio_index)
	#define __STM32_PIN_DEFAULT 0

	#define A   (1U << 8)
	#define B   (2U << 8)
	#define C   (3U << 8)
	#define D   (4U << 8)
	#define E   (5U << 8)
	#define F   (6U << 8)
	#define G   (7U << 8)
	#define H   (8U << 8)
	#define I   (9U << 8)
	#define J   (10U << 8)
	#define K   (11U << 8)

	static GPIO_TypeDef * get_st_gpio(uint16_t gpio_pin)
	{
			switch(gpio_pin & 0xFF00)
			{
			case A:
					#ifdef GPIOA
					return GPIOA;
					#endif
			case B:
					#ifdef GPIOB
					return GPIOB;
					#endif
			case C:
					#ifdef GPIOC
					return GPIOC;
					#endif
			case D:
					#ifdef GPIOD
					return GPIOD;
					#endif
			case E:
					#ifdef GPIOE
					return GPIOE;
					#endif
			case F:
					#ifdef GPIOF
					return GPIOF;
					#endif
			case G:
					#ifdef GPIOG
					return GPIOG;
					#endif
			case H:
					#ifdef GPIOH
					return GPIOH;
					#endif
			case I:
					#ifdef GPIOI
					return GPIOI;
					#endif
			case J:
					#ifdef GPIOJ
					return GPIOJ;
					#endif
			case K:
					#ifdef GPIOK
					return GPIOK;
					#endif
			default:
					return NULL;
			}
	}


	#define get_st_pin(gpio_pin) (0x01 << (gpio_pin&0xFF))

	static void drv_clock_enable(uint16_t gpio_pin)
	{
			switch(gpio_pin & 0xFF00)
			{
			case A:
					#ifdef __HAL_RCC_GPIOA_CLK_ENABLE
					__HAL_RCC_GPIOA_CLK_ENABLE();
					#endif
					break;
			case B:
					#ifdef __HAL_RCC_GPIOB_CLK_ENABLE
					__HAL_RCC_GPIOB_CLK_ENABLE();
					#endif
					break;
			case C:
					#ifdef __HAL_RCC_GPIOC_CLK_ENABLE
					__HAL_RCC_GPIOC_CLK_ENABLE();
					#endif
					break;
			case D:
					#ifdef __HAL_RCC_GPIOD_CLK_ENABLE
					__HAL_RCC_GPIOD_CLK_ENABLE();
					#endif
					break;
			case E:
					#ifdef __HAL_RCC_GPIOE_CLK_ENABLE
					__HAL_RCC_GPIOE_CLK_ENABLE();
					#endif
					break;
			case F:
					#ifdef __HAL_RCC_GPIOF_CLK_ENABLE
					__HAL_RCC_GPIOF_CLK_ENABLE();
					#endif
					break;
			case G:
					#ifdef __HAL_RCC_GPIOG_CLK_ENABLE
					__HAL_RCC_GPIOG_CLK_ENABLE();
					#endif
					break;
			case H:
					#ifdef __HAL_RCC_GPIOH_CLK_ENABLE
					__HAL_RCC_GPIOH_CLK_ENABLE();
					#endif
					break;
			case I:
					#ifdef __HAL_RCC_GPIOI_CLK_ENABLE
					__HAL_RCC_GPIOI_CLK_ENABLE();
					#endif
					break;
			case J:
					#ifdef __HAL_RCC_GPIOJ_CLK_ENABLE
					__HAL_RCC_GPIOJ_CLK_ENABLE();
					#endif
					break;
			case K:
					#ifdef __HAL_RCC_GPIOK_CLK_ENABLE
					__HAL_RCC_GPIOK_CLK_ENABLE();
					#endif
					break;
			default:
					break;
			}
	}

	static const rt_uint16_t pins[] =
	{
 
		
#ifdef USING_STM32F030F4
		__STM32_PIN_DEFAULT,
		__STM32_PIN_DEFAULT,
		__STM32_PIN(2, F, 0),
		__STM32_PIN(3, F, 1),
		__STM32_PIN_DEFAULT,
		__STM32_PIN_DEFAULT,
		__STM32_PIN(6, A, 0),
		__STM32_PIN(7, A, 1),
		__STM32_PIN(8, A, 2),
		__STM32_PIN(9, A, 3),	
		__STM32_PIN(10, A, 4),
		__STM32_PIN(11, A, 5),
		__STM32_PIN(12, A, 6),
		__STM32_PIN(13, A, 7),
		__STM32_PIN(14, B, 1),
		__STM32_PIN_DEFAULT,
		__STM32_PIN_DEFAULT,
		__STM32_PIN(17, A, 9),
		__STM32_PIN(18, A, 10),
		__STM32_PIN(19, A, 13),
		__STM32_PIN(20, A, 14),
#endif
		
	#if (STM32F10X_PIN_NUMBERS == 36)
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(7, A, 0),
			__STM32_PIN(8, A, 1),
			__STM32_PIN(9, A, 2),
			__STM32_PIN(10, A, 3),
			__STM32_PIN(11, A, 4),
			__STM32_PIN(12, A, 5),
			__STM32_PIN(13, A, 6),
			__STM32_PIN(14, A, 7),
			__STM32_PIN(15, B, 0),
			__STM32_PIN(16, B, 1),
			__STM32_PIN(17, B, 2),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(20, A, 8),
			__STM32_PIN(21, A, 9),
			__STM32_PIN(22, A, 10),
			__STM32_PIN(23, A, 11),
			__STM32_PIN(24, A, 12),
			__STM32_PIN(25, A, 13),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(28, A, 14),
			__STM32_PIN(29, A, 15),
			__STM32_PIN(30, B, 3),
			__STM32_PIN(31, B, 4),
			__STM32_PIN(32, B, 5),
			__STM32_PIN(33, B, 6),
			__STM32_PIN(34, B, 7),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
	#endif
	#if (STM32F10X_PIN_NUMBERS == 48)
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(2, C, 13),
			__STM32_PIN(3, C, 14),
			__STM32_PIN(4, C, 15),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(10, A, 0),
			__STM32_PIN(11, A, 1),
			__STM32_PIN(12, A, 2),
			__STM32_PIN(13, A, 3),
			__STM32_PIN(14, A, 4),
			__STM32_PIN(15, A, 5),
			__STM32_PIN(16, A, 6),
			__STM32_PIN(17, A, 7),
			__STM32_PIN(18, B, 0),
			__STM32_PIN(19, B, 1),
			__STM32_PIN(20, B, 2),
			__STM32_PIN(21, B, 10),
			__STM32_PIN(22, B, 11),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(25, B, 12),
			__STM32_PIN(26, B, 13),
			__STM32_PIN(27, B, 14),
			__STM32_PIN(28, B, 15),
			__STM32_PIN(29, A, 8),
			__STM32_PIN(30, A, 9),
			__STM32_PIN(31, A, 10),
			__STM32_PIN(32, A, 11),
			__STM32_PIN(33, A, 12),
			__STM32_PIN(34, A, 13),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(37, A, 14),
			__STM32_PIN(38, A, 15),
			__STM32_PIN(39, B, 3),
			__STM32_PIN(40, B, 4),
			__STM32_PIN(41, B, 5),
			__STM32_PIN(42, B, 6),
			__STM32_PIN(43, B, 7),
			__STM32_PIN_DEFAULT,
			__STM32_PIN(45, B, 8),
			__STM32_PIN(46, B, 9),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,

	#endif
	#if (STM32F10X_PIN_NUMBERS == 64)
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(2, C, 13),
			__STM32_PIN(3, C, 14),
			__STM32_PIN(4, C, 15),
			__STM32_PIN(5, D, 0),
			__STM32_PIN(6, D, 1),
			__STM32_PIN_DEFAULT,
			__STM32_PIN(8, C, 0),
			__STM32_PIN(9, C, 1),
			__STM32_PIN(10, C, 2),
			__STM32_PIN(11, C, 3),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(14, A, 0),
			__STM32_PIN(15, A, 1),
			__STM32_PIN(16, A, 2),
			__STM32_PIN(17, A, 3),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(20, A, 4),
			__STM32_PIN(21, A, 5),
			__STM32_PIN(22, A, 6),
			__STM32_PIN(23, A, 7),
			__STM32_PIN(24, C, 4),
			__STM32_PIN(25, C, 5),
			__STM32_PIN(26, B, 0),
			__STM32_PIN(27, B, 1),
			__STM32_PIN(28, B, 2),
			__STM32_PIN(29, B, 10),
			__STM32_PIN(30, B, 11),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(33, B, 12),
			__STM32_PIN(34, B, 13),
			__STM32_PIN(35, B, 14),
			__STM32_PIN(36, B, 15),
			__STM32_PIN(37, C, 6),
			__STM32_PIN(38, C, 7),
			__STM32_PIN(39, C, 8),
			__STM32_PIN(40, C, 9),
			__STM32_PIN(41, A, 8),
			__STM32_PIN(42, A, 9),
			__STM32_PIN(43, A, 10),
			__STM32_PIN(44, A, 11),
			__STM32_PIN(45, A, 12),
			__STM32_PIN(46, A, 13),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(49, A, 14),
			__STM32_PIN(50, A, 15),
			__STM32_PIN(51, C, 10),
			__STM32_PIN(52, C, 11),
			__STM32_PIN(53, C, 12),
			__STM32_PIN(54, D, 2),
			__STM32_PIN(55, B, 3),
			__STM32_PIN(56, B, 4),
			__STM32_PIN(57, B, 5),
			__STM32_PIN(58, B, 6),
			__STM32_PIN(59, B, 7),
			__STM32_PIN_DEFAULT,
			__STM32_PIN(61, B, 8),
			__STM32_PIN(62, B, 9),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
	#endif
	#if (STM32F10X_PIN_NUMBERS == 100)
			__STM32_PIN_DEFAULT,
			__STM32_PIN(1, E, 2),
			__STM32_PIN(2, E, 3),
			__STM32_PIN(3, E, 4),
			__STM32_PIN(4, E, 5),
			__STM32_PIN(5, E, 6),
			__STM32_PIN_DEFAULT,
			__STM32_PIN(7, C, 13),
			__STM32_PIN(8, C, 14),
			__STM32_PIN(9, C, 15),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(15, C, 0),
			__STM32_PIN(16, C, 1),
			__STM32_PIN(17, C, 2),
			__STM32_PIN(18, C, 3),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(23, A, 0),
			__STM32_PIN(24, A, 1),
			__STM32_PIN(25, A, 2),
			__STM32_PIN(26, A, 3),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(29, A, 4),
			__STM32_PIN(30, A, 5),
			__STM32_PIN(31, A, 6),
			__STM32_PIN(32, A, 7),
			__STM32_PIN(33, C, 4),
			__STM32_PIN(34, C, 5),
			__STM32_PIN(35, B, 0),
			__STM32_PIN(36, B, 1),
			__STM32_PIN(37, B, 2),
			__STM32_PIN(38, E, 7),
			__STM32_PIN(39, E, 8),
			__STM32_PIN(40, E, 9),
			__STM32_PIN(41, E, 10),
			__STM32_PIN(42, E, 11),
			__STM32_PIN(43, E, 12),
			__STM32_PIN(44, E, 13),
			__STM32_PIN(45, E, 14),
			__STM32_PIN(46, E, 15),
			__STM32_PIN(47, B, 10),
			__STM32_PIN(48, B, 11),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(51, B, 12),
			__STM32_PIN(52, B, 13),
			__STM32_PIN(53, B, 14),
			__STM32_PIN(54, B, 15),
			__STM32_PIN(55, D, 8),
			__STM32_PIN(56, D, 9),
			__STM32_PIN(57, D, 10),
			__STM32_PIN(58, D, 11),
			__STM32_PIN(59, D, 12),
			__STM32_PIN(60, D, 13),
			__STM32_PIN(61, D, 14),
			__STM32_PIN(62, D, 15),
			__STM32_PIN(63, C, 6),
			__STM32_PIN(64, C, 7),
			__STM32_PIN(65, C, 8),
			__STM32_PIN(66, C, 9),
			__STM32_PIN(67, A, 8),
			__STM32_PIN(68, A, 9),
			__STM32_PIN(69, A, 10),
			__STM32_PIN(70, A, 11),
			__STM32_PIN(71, A, 12),
			__STM32_PIN(72, A, 13),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(76, A, 14),
			__STM32_PIN(77, A, 15),
			__STM32_PIN(78, C, 10),
			__STM32_PIN(79, C, 11),
			__STM32_PIN(80, C, 12),
			__STM32_PIN(81, D, 0),
			__STM32_PIN(82, D, 1),
			__STM32_PIN(83, D, 2),
			__STM32_PIN(84, D, 3),
			__STM32_PIN(85, D, 4),
			__STM32_PIN(86, D, 5),
			__STM32_PIN(87, D, 6),
			__STM32_PIN(88, D, 7),
			__STM32_PIN(89, B, 3),
			__STM32_PIN(90, B, 4),
			__STM32_PIN(91, B, 5),
			__STM32_PIN(92, B, 6),
			__STM32_PIN(93, B, 7),
			__STM32_PIN_DEFAULT,
			__STM32_PIN(95, B, 8),
			__STM32_PIN(96, B, 9),
			__STM32_PIN(97, E, 0),
			__STM32_PIN(98, E, 1),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
	#endif
	#if (STM32F10X_PIN_NUMBERS == 144)
			__STM32_PIN_DEFAULT,
			__STM32_PIN(1, E, 2),
			__STM32_PIN(2, E, 3),
			__STM32_PIN(3, E, 4),
			__STM32_PIN(4, E, 5),
			__STM32_PIN(5, E, 6),
			__STM32_PIN_DEFAULT,
			__STM32_PIN(7, C, 13),
			__STM32_PIN(8, C, 14),
			__STM32_PIN(9, C, 15),
			__STM32_PIN(10, F, 0),
			__STM32_PIN(11, F, 1),
			__STM32_PIN(12, F, 2),
			__STM32_PIN(13, F, 3),
			__STM32_PIN(14, F, 4),
			__STM32_PIN(15, F, 5),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(18, F, 6),
			__STM32_PIN(19, F, 7),
			__STM32_PIN(20, F, 8),
			__STM32_PIN(21, F, 9),
			__STM32_PIN(22, F, 10),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(26, C, 0),
			__STM32_PIN(27, C, 1),
			__STM32_PIN(28, C, 2),
			__STM32_PIN(29, C, 3),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(34, A, 0),
			__STM32_PIN(35, A, 1),
			__STM32_PIN(36, A, 2),
			__STM32_PIN(37, A, 3),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(40, A, 4),
			__STM32_PIN(41, A, 5),
			__STM32_PIN(42, A, 6),
			__STM32_PIN(43, A, 7),
			__STM32_PIN(44, C, 4),
			__STM32_PIN(45, C, 5),
			__STM32_PIN(46, B, 0),
			__STM32_PIN(47, B, 1),
			__STM32_PIN(48, B, 2),
			__STM32_PIN(49, F, 11),
			__STM32_PIN(50, F, 12),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(53, F, 13),
			__STM32_PIN(54, F, 14),
			__STM32_PIN(55, F, 15),
			__STM32_PIN(56, G, 0),
			__STM32_PIN(57, G, 1),
			__STM32_PIN(58, E, 7),
			__STM32_PIN(59, E, 8),
			__STM32_PIN(60, E, 9),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(63, E, 10),
			__STM32_PIN(64, E, 11),
			__STM32_PIN(65, E, 12),
			__STM32_PIN(66, E, 13),
			__STM32_PIN(67, E, 14),
			__STM32_PIN(68, E, 15),
			__STM32_PIN(69, B, 10),
			__STM32_PIN(70, B, 11),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(73, B, 12),
			__STM32_PIN(74, B, 13),
			__STM32_PIN(75, B, 14),
			__STM32_PIN(76, B, 15),
			__STM32_PIN(77, D, 8),
			__STM32_PIN(78, D, 9),
			__STM32_PIN(79, D, 10),
			__STM32_PIN(80, D, 11),
			__STM32_PIN(81, D, 12),
			__STM32_PIN(82, D, 13),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(85, D, 14),
			__STM32_PIN(86, D, 15),
			__STM32_PIN(87, G, 2),
			__STM32_PIN(88, G, 3),
			__STM32_PIN(89, G, 4),
			__STM32_PIN(90, G, 5),
			__STM32_PIN(91, G, 6),
			__STM32_PIN(92, G, 7),
			__STM32_PIN(93, G, 8),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(96, C, 6),
			__STM32_PIN(97, C, 7),
			__STM32_PIN(98, C, 8),
			__STM32_PIN(99, C, 9),
			__STM32_PIN(100, A, 8),
			__STM32_PIN(101, A, 9),
			__STM32_PIN(102, A, 10),
			__STM32_PIN(103, A, 11),
			__STM32_PIN(104, A, 12),
			__STM32_PIN(105, A, 13),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(109, A, 14),
			__STM32_PIN(110, A, 15),
			__STM32_PIN(111, C, 10),
			__STM32_PIN(112, C, 11),
			__STM32_PIN(113, C, 12),
			__STM32_PIN(114, D, 0),
			__STM32_PIN(115, D, 1),
			__STM32_PIN(116, D, 2),
			__STM32_PIN(117, D, 3),
			__STM32_PIN(118, D, 4),
			__STM32_PIN(119, D, 5),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(122, D, 6),
			__STM32_PIN(123, D, 7),
			__STM32_PIN(124, G, 9),
			__STM32_PIN(125, G, 10),
			__STM32_PIN(126, G, 11),
			__STM32_PIN(127, G, 12),
			__STM32_PIN(128, G, 13),
			__STM32_PIN(129, G, 14),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
			__STM32_PIN(132, G, 15),
			__STM32_PIN(133, B, 3),
			__STM32_PIN(134, B, 4),
			__STM32_PIN(135, B, 5),
			__STM32_PIN(136, B, 6),
			__STM32_PIN(137, B, 7),
			__STM32_PIN_DEFAULT,
			__STM32_PIN(139, B, 8),
			__STM32_PIN(140, B, 9),
			__STM32_PIN(141, E, 0),
			__STM32_PIN(142, E, 1),
			__STM32_PIN_DEFAULT,
			__STM32_PIN_DEFAULT,
	#endif
#ifdef GPIOA
    __STM32_PIN(0 ,  A, 0 ),
    __STM32_PIN(1 ,  A, 1 ),
    __STM32_PIN(2 ,  A, 2 ),
    __STM32_PIN(3 ,  A, 3 ),
    __STM32_PIN(4 ,  A, 4 ),
    __STM32_PIN(5 ,  A, 5 ),
    __STM32_PIN(6 ,  A, 6 ),
    __STM32_PIN(7 ,  A, 7 ),
    __STM32_PIN(8 ,  A, 8 ),
    __STM32_PIN(9 ,  A, 9 ),
    __STM32_PIN(10,  A, 10),
    __STM32_PIN(11,  A, 11),
    __STM32_PIN(12,  A, 12),
    __STM32_PIN(13,  A, 13),
    __STM32_PIN(14,  A, 14),
    __STM32_PIN(15,  A, 15),
#endif
#ifdef GPIOB
    __STM32_PIN(16,  B, 0),
    __STM32_PIN(17,  B, 1),
    __STM32_PIN(18,  B, 2),
    __STM32_PIN(19,  B, 3),
    __STM32_PIN(20,  B, 4),
    __STM32_PIN(21,  B, 5),
    __STM32_PIN(22,  B, 6),
    __STM32_PIN(23,  B, 7),
    __STM32_PIN(24,  B, 8),
    __STM32_PIN(25,  B, 9),
    __STM32_PIN(26,  B, 10),
    __STM32_PIN(27,  B, 11),
    __STM32_PIN(28,  B, 12),
    __STM32_PIN(29,  B, 13),
    __STM32_PIN(30,  B, 14),
    __STM32_PIN(31,  B, 15),
#endif
#ifdef GPIOC
    __STM32_PIN(32,  C, 0),
    __STM32_PIN(33,  C, 1),
    __STM32_PIN(34,  C, 2),
    __STM32_PIN(35,  C, 3),
    __STM32_PIN(36,  C, 4),
    __STM32_PIN(37,  C, 5),
    __STM32_PIN(38,  C, 6),
    __STM32_PIN(39,  C, 7),
    __STM32_PIN(40,  C, 8),
    __STM32_PIN(41,  C, 9),
    __STM32_PIN(42,  C, 10),
    __STM32_PIN(43,  C, 11),
    __STM32_PIN(44,  C, 12),
    __STM32_PIN(45,  C, 13),
    __STM32_PIN(46,  C, 14),
    __STM32_PIN(47,  C, 15),
#endif
#ifdef GPIOD
    __STM32_PIN(48,  D, 0),
    __STM32_PIN(49,  D, 1),
    __STM32_PIN(50,  D, 2),
    __STM32_PIN(51,  D, 3),
    __STM32_PIN(52,  D, 4),
    __STM32_PIN(53,  D, 5),
    __STM32_PIN(54,  D, 6),
    __STM32_PIN(55,  D, 7),
    __STM32_PIN(56,  D, 8),
    __STM32_PIN(57,  D, 9),
    __STM32_PIN(58,  D, 10),
    __STM32_PIN(59,  D, 11),
    __STM32_PIN(60,  D, 12),
    __STM32_PIN(61,  D, 13),
    __STM32_PIN(62,  D, 14),
    __STM32_PIN(63,  D, 15),
#endif
#ifdef GPIOE
    __STM32_PIN(64,  E, 0),
    __STM32_PIN(65,  E, 1),
    __STM32_PIN(66,  E, 2),
    __STM32_PIN(67,  E, 3),
    __STM32_PIN(68,  E, 4),
    __STM32_PIN(69,  E, 5),
    __STM32_PIN(70,  E, 6),
    __STM32_PIN(71,  E, 7),
    __STM32_PIN(72,  E, 8),
    __STM32_PIN(73,  E, 9),
    __STM32_PIN(74,  E, 10),
    __STM32_PIN(75,  E, 11),
    __STM32_PIN(76,  E, 12),
    __STM32_PIN(77,  E, 13),
    __STM32_PIN(78,  E, 14),
    __STM32_PIN(79,  E, 15),
#endif
#ifdef GPIOF
    __STM32_PIN(80,  F, 0),
    __STM32_PIN(81,  F, 1),
    __STM32_PIN(82,  F, 2),
    __STM32_PIN(83,  F, 3),
    __STM32_PIN(84,  F, 4),
    __STM32_PIN(85,  F, 5),
    __STM32_PIN(86,  F, 6),
    __STM32_PIN(87,  F, 7),
    __STM32_PIN(88,  F, 8),
    __STM32_PIN(89,  F, 9),
    __STM32_PIN(90,  F, 10),
    __STM32_PIN(91,  F, 11),
    __STM32_PIN(92,  F, 12),
    __STM32_PIN(93,  F, 13),
    __STM32_PIN(94,  F, 14),
    __STM32_PIN(95,  F, 15),
#endif
#ifdef GPIOG
    __STM32_PIN(96,  G, 0),
    __STM32_PIN(97,  G, 1),
    __STM32_PIN(98,  G, 2),
    __STM32_PIN(99,  G, 3),
    __STM32_PIN(100, G, 4),
    __STM32_PIN(101, G, 5),
    __STM32_PIN(102, G, 6),
    __STM32_PIN(103, G, 7),
    __STM32_PIN(104, G, 8),
    __STM32_PIN(105, G, 9),
    __STM32_PIN(106, G, 10),
    __STM32_PIN(107, G, 11),
    __STM32_PIN(108, G, 12),
    __STM32_PIN(109, G, 13),
    __STM32_PIN(110, G, 14),
    __STM32_PIN(111, G, 15),
#endif
#ifdef GPIOH
    __STM32_PIN(112, H, 0),
    __STM32_PIN(113, H, 1),
    __STM32_PIN(114, H, 2),
    __STM32_PIN(115, H, 3),
    __STM32_PIN(116, H, 4),
    __STM32_PIN(117, H, 5),
    __STM32_PIN(118, H, 6),
    __STM32_PIN(119, H, 7),
    __STM32_PIN(120, H, 8),
    __STM32_PIN(121, H, 9),
    __STM32_PIN(122, H, 10),
    __STM32_PIN(124, H, 11),
    __STM32_PIN(125, H, 12),
    __STM32_PIN(126, H, 13),
    __STM32_PIN(127, H, 14),
    __STM32_PIN(128, H, 15),
#endif
#ifdef GPIOI
    __STM32_PIN(129, I, 0),
    __STM32_PIN(130, I, 1),
    __STM32_PIN(131, I, 2),
    __STM32_PIN(132, I, 3),
    __STM32_PIN(133, I, 4),
    __STM32_PIN(134, I, 5),
    __STM32_PIN(135, I, 6),
    __STM32_PIN(136, I, 7),
    __STM32_PIN(137, I, 8),
    __STM32_PIN(138, I, 9),
    __STM32_PIN(139, I, 10),
    __STM32_PIN(140, I, 11),
    __STM32_PIN(141, I, 12),
    __STM32_PIN(142, I, 13),
    __STM32_PIN(143, I, 14),
    __STM32_PIN(144, I, 15),
#endif
#ifdef GPIOJ
    __STM32_PIN(145, J, 0),
    __STM32_PIN(146, J, 1),
    __STM32_PIN(147, J, 2),
    __STM32_PIN(148, J, 3),
    __STM32_PIN(149, J, 4),
    __STM32_PIN(150, J, 5),
    __STM32_PIN(151, J, 6),
    __STM32_PIN(152, J, 7),
    __STM32_PIN(153, J, 8),
    __STM32_PIN(154, J, 9),
    __STM32_PIN(155, J, 10),
    __STM32_PIN(156, J, 11),
    __STM32_PIN(157, J, 12),
    __STM32_PIN(158, J, 13),
    __STM32_PIN(159, J, 14),
    __STM32_PIN(160, J, 15),
#endif
#ifdef GPIOK
    __STM32_PIN(161, K, 0),
    __STM32_PIN(162, K, 1),
    __STM32_PIN(163, K, 2),
    __STM32_PIN(164, K, 3),
    __STM32_PIN(165, K, 4),
    __STM32_PIN(166, K, 5),
    __STM32_PIN(167, K, 6),
    __STM32_PIN(168, K, 7),
    __STM32_PIN(169, K, 8),
    __STM32_PIN(170, K, 9),
    __STM32_PIN(171, K, 10),
    __STM32_PIN(172, K, 11),
    __STM32_PIN(173, K, 12),
    __STM32_PIN(174, K, 13),
    __STM32_PIN(175, K, 14),
    __STM32_PIN(176, K, 15),
#endif	
	
	};
	#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])
	static uint16_t get_pin(uint8_t pin)
	{
			uint16_t gpio_pin = __STM32_PIN_DEFAULT;
			if (pin < ITEM_NUM(pins))
			{
					gpio_pin = pins[pin];
			}
			return gpio_pin;
	};
	//
#endif
	
void pinMode(uint8_t pin, uint8_t mode)
{
	#if defined(USING_RTT)
		rt_pin_mode(pin,mode);
	#else
		uint16_t gpio_pin;
		GPIO_InitTypeDef GPIO_InitStruct;
		gpio_pin = get_pin(pin);
    if (get_st_gpio(gpio_pin) == NULL)
    {
        return;
    }	
    /* GPIO Periph clock enable */
    drv_clock_enable(gpio_pin);
    /* Configure GPIO_InitStructure */
    GPIO_InitStruct.Pin = get_st_pin(gpio_pin);
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    if (mode == PIN_MODE_INPUT)
    {
        /* input setting: not pull. */
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    }
    else if (mode == PIN_MODE_INPUT_PULLUP)
    {
        /* input setting: pull up. */
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
    }
    else if (mode == PIN_MODE_INPUT_PULLDOWN)
    {
        /* input setting: pull down. */
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    }
    else if (mode == PIN_MODE_OUTPUT_OD)
    {
        /* output setting: od. */
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    }
    HAL_GPIO_Init(get_st_gpio(gpio_pin), &GPIO_InitStruct);	
		#endif
}
//

void pinSet(uint8_t pin)
{
	#if defined(USING_RTT)
	rt_pin_write(pin, PIN_HIGH);
	#else
	rt_uint16_t gpio_pin;
	gpio_pin = get_pin(pin);
	if (get_st_gpio(gpio_pin) == RT_NULL)
	{
			return;
	}
	HAL_GPIO_WritePin(get_st_gpio(gpio_pin), get_st_pin(gpio_pin), (GPIO_PinState)PIN_HIGH);
	#endif
}
void pinReset(uint8_t pin)
{
	#if defined(USING_RTT)
	rt_pin_write(pin, PIN_LOW);
	#else
	rt_uint16_t gpio_pin;
	gpio_pin = get_pin(pin);
	if (get_st_gpio(gpio_pin) == NULL)
	{
			return;
	}
	HAL_GPIO_WritePin(get_st_gpio(gpio_pin), get_st_pin(gpio_pin), (GPIO_PinState)PIN_LOW);
	#endif
}
void pinWrite(uint8_t pin,uint8_t val)
{
	 if(val == 0)   pinReset(pin);
	 else						pinSet(pin);
}
uint8_t pinRead(uint8_t pin)
{
	#if defined(USING_RTT)
		if(rt_pin_read(pin)) return  1;
		else 								 return  0;
	#else
    rt_uint16_t gpio_pin;
    gpio_pin = get_pin(pin);
    if (get_st_gpio(gpio_pin) == NULL)
    {
        return PIN_LOW;
    }
    return HAL_GPIO_ReadPin(get_st_gpio(gpio_pin), get_st_pin(gpio_pin));
	#endif
}
void pinToggle(uint8_t pin)
{
	#if defined(USING_RTT)
	rt_pin_write(pin, 1 - rt_pin_read(pin));
	#else
	pinWrite(pin, 1 - pinRead(pin));
	#endif
}

uint8_t shift_in(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order)
{
    uint8_t value = 0;
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
       if (bit_order == LSB_FIRST)      value |= pinRead(data_pin) << i;
       else									            value |= pinRead(data_pin) << (7 - i);
       pinWrite(	clock_pin,	LOW		);	drv_udelay(1);
			 pinWrite(	clock_pin,	HIGH	);	drv_udelay(1);
    }
    return value;
}

void shift_out(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order, uint8_t val)
{
	int i;
	for (i = 0; i < 8; i++)
	{
			if (bit_order == LSB_FIRST)
					pinWrite(	data_pin,	(!!(val & (1 << i))));
			else
					pinWrite(	data_pin,(!!(val & (1 << (7 - i))))	);

			pinWrite(	clock_pin,	HIGH	);
			pinWrite(	clock_pin,	LOW		);
	}
}

