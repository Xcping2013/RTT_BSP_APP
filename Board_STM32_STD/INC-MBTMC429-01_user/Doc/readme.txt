20190418: 速度、加速度;外部过压中断;SPI;SetUpAfterHomed;

把STD3.5库版本也移植到BSP APPP下进行管理，新增分支管理  不同分支，相同文件名，不一样的文件内容---配置代码

代码hex传输，需要思勇更新至网盘，后期关于其他项目电机未到位或者修改情况需要售后重新新代码

communication code : buffer, handshake, done words words words！！！

产线现象与问题处理：
1.过压：不同的产品，压力值不同，有些产品的压力值过大，会压坏产品
  压力仪设置上限报警功能，控制板一采集到报警信号立刻控制电机停止运动，并发送信息给软件，软件采集后让电机回原点
  
2.测试过程中电机停止，但软件流程在跑，此时不能按复位按键，需等待软件自动回原点或者重启软件

ReadWriteSPI;
EXTI2_IRQHandler; 下降沿;  NVIC_IRQChannelPreemptionPriority

电机：顺时针 正转 R；逆时针反转	L	

20190414：增加外部中断，24V输入即GPIO为H时采集到过压信号
		 （默认采集到0V）L->H上升沿触发
		  InitIn8AsExti();


IO扩展板丝印 ：  输入正序IN13---INPUT13    输出反序OUT9---OUTPUT14

1.双启按键必须为2个按键一起同时按下才动作，当双启1按下后超过30ms再按下双启按键2后为无效动作
2.双启按键触发后的2个气缸动作必须完成后才会进行测试动作
3.修复lidopen双启动作后的采集命令
4.增加LCR coil项目切换命令EPP-IN3WHIS18R-001	
5.不同项目独立对应.C文件与其他框架分开
																--20180228
1.增加四个GPIO用于与GND的切换								    --20180314

1.四个GPIO改为两个LED2和IN3
2.项目切换命令更改为:	type[button online]    type[button offline]    type[lidopen]    type[iac lcr]    type[jabil lcr]
																--20180323	
																
待优化：接收命令固定为小写，发送命令与返回命令为小写;命令格式优化成与INC-MB1616007-B188&B235一样，不必返回2次发生值
	old:int[1]\r\n		int[1]<1>					new: int[1]<1>;		 read_input(1)<1>;		
	old:out[1][1]\r\n	out[1][1]<ok>				new: out[1][1]<ok>;	 set_output(1,1)<ok>

修改SPI发送和接收函数，保证不贴SPI器件时，SPI能自动延时退出，PCB需要增加电容，防止热拔插打坏IC，焊接或者接线错误IC故障
															 ------20180329
															 
IN OUT LED引脚改变	;	SPI1_CS IIC 引脚改变	

EEPROM	AT24C128--->256

LCR Contro

