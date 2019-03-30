drv_gpio
{
		rt_hw_pin_init
				//结构体填充
				rt_device_pin_register("pin", &_stm32_pin_ops, RT_NULL);
				/*
						_hw_pin.parent.read         = _pin_read;
						_hw_pin.parent.write        = _pin_write;
						_hw_pin.parent.control      = _pin_control;
						_hw_pin.ops 								= ops;
				*/
				/*
				struct rt_device_pin
				{
					struct rt_device parent;
					const struct rt_pin_ops *ops;
				};
				static struct rt_device_pin _hw_pin;
				*/
				rt_pin_mode --- _hw_pin.ops->pin_mode	--- _stm32_pin_ops._stm32_pin_ops
						rt_device_register(&_hw_pin.parent, name, RT_DEVICE_FLAG_RDWR);
			
		EXTI0_IRQHandler
			HAL_GPIO_EXTI_Callback		
}

drv_usart//serial 的填充
{
	struct rt_serial_device serial1;
	struct stm32_uart uart1 =
	{
			{USART1},
			USART1_IRQn
	};
	static const struct rt_uart_ops stm32_uart_ops =
	{
			stm32_configure,
			stm32_control,
			stm32_putc,
			stm32_getc,
	};

	rt_hw_usart_init
	{
			struct stm32_uart	*uart;
			{
					UART_HandleTypeDef huart;
					IRQn_Type irq;
			};
			struct serial_configure	config
			{
					rt_uint32_t baud_rate;

					rt_uint32_t data_bits               :4;
					rt_uint32_t stop_bits               :2;
					rt_uint32_t parity                  :2;
					rt_uint32_t bit_order               :1;
					rt_uint32_t invert                  :1;
					rt_uint32_t bufsz                   :16;
					rt_uint32_t reserved                :4;
			};
			serial1.config = config;
			 rt_hw_serial_register(&serial1, "uart1",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          uart);
			 {
				 device = &(serial->parent);
				    device->init        = rt_serial_init;
						device->open        = rt_serial_open;
						device->close       = rt_serial_close;
						device->read        = rt_serial_read;
						device->write       = rt_serial_write;
						device->control     = rt_serial_control;
				 rt_device_register(device, name, flag);			
			 }
	}
}
drv_spi
{
	#define SPI_BUS_NAME          "spi1"
	#define SPI_DEVICE1_NAME      "spi1_dev1"
	struct stm32_hw_spi_cs
	{
			rt_uint32_t pin;
	};
static struct rt_spi_device spi_dev_spidev1; 
static struct stm32_hw_spi_cs  spi_cs;  

	static int rt_hw_spidev1_init(void)
	{
		rt_hw_spidev1_config()
		{
			#define CS_PIN   98
			spi_cs.pin = CS_PIN;
			rt_pin_mode(spi_cs.pin, PIN_MODE_OUTPUT);
			rt_spi_bus_attach_device(&spi_dev_spidev1, SPI_DEVICE1_NAME, SPI_BUS_NAME, (void *)&spi_cs);
			/* config spi */
			{
					struct rt_spi_configuration cfg;
					cfg.data_width = 8;
					cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
					cfg.max_hz = 20 * 1000 * 1000; /* 20M,SPI max 42MHz,ssd1351 4-wire spi */
					rt_spi_configure(&spi_dev_spidev1, &cfg);
			}
		}
	}
	rt_spi_send(&spi_dev_spidev1, &cmd, 1);
}
//
drv_i2c
{
	#define I2CDEV1_ADDR                0X68
	#define I2CDEV1_I2CBUS_NAME  			"i2c2"     /* I2C设备名称,必须和drv_i2c.c注册的I2C设备名称一致 */
	static struct rt_i2c_bus_device *i2cddev1_i2c_bus;    /* I2C设备句柄 */
	int mpu6050_hw_init(void)
	{
		i2cddev1_i2c_bus = rt_i2c_bus_device_find(I2CDEV1_I2CBUS_NAME);  /*查找I2C设备*/
		
	}
·rt_err_t i2cdev1_write_reg(rt_uint8_t reg, rt_uint8_t data)
	{
    rt_uint8_t buf[2];

    buf[0] = reg;
    buf[1] = data;

    if (rt_i2c_master_send(i2cdev1_i2c_bus, I2CDEV1_ADDR, 0, buf, 2) == 2)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
	}	
	rt_err_t i2cdev1_read_reg(rt_uint8_t reg, rt_uint8_t len, rt_uint8_t *buf)
	{
			if (rt_i2c_master_send(i2cdev1_i2c_bus, I2CDEV1_ADDR, 0, &reg, 1) == 1)
			{
					if (rt_i2c_master_recv(i2cdev1_i2c_bus, I2CDEV1_ADDR, 0, buf, len) == len)
					{
							return RT_EOK;
					}
					else
					{
							return -RT_ERROR;
					}
			}
			else
			{
					return -RT_ERROR;
			}

	}	
}
//





