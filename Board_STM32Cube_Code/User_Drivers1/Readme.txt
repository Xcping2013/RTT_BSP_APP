自己的GPIO底层无需使用RTT驱动，这样HAL和STD就可以统一 RTT自带IIC等驱动 ---- 自己不要再浪费时间

基本应用UART_CONSOLE	+	iic	eeprom	+  spi drivers

使用RTT也可以不使用SHELL 使用自定义console ,也可以使用rtt msh	这样就可以兼容user和函数格式命令

小结： 默认使用rtt msh	需要时可切换为finsh

iic-spi的应用