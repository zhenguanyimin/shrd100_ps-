20230726-1230(版本号：0004025407261230)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本	
	1.基于宋工维护droneid最新工程，版本号：0004025207181146进行修改；
	2.合并频谱侦测(优化功能)+WIFI机型功能；
	3.体现droneid的功能，已测无人机进行验证通过；
	4.频谱侦测+WIFI机型的功能，已测无人机进行验证通过；


20230726-1630(版本号：0004125407261630)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本	
	1.基于宋工维护droneid最新工程，版本号：0004025207181146进行修改；
	2.合并频谱侦测(优化功能)+WIFI机型功能；
	3.体现频谱侦测+WIFI机型的功能，待验证；
	
-----------------------------------------------------------------------------------------------------------
后期版本号更新成如下格式
示例版本号：0x:00040726(0A/0B)030010
			0x:0004：Tracer
			0x:0726：月+日
			0x:0Axx：8bit(0A:droneid 			)+8bit(版本号)
			0x:0Bxx：8bit(0B:频谱侦测+WIFI机型  )+8bit(版本号)
			0x:xxxx：16bit(版本号)
	


20230727-1830(版本号：00040727_0A030012)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.基于最新工程，版本号：0004025407261230进行修改；
	2.在BD模块新增3GPP Mixed Mode turbo decoder IP核 ；
	3.修改UART0-UART6:的串口波特率，统一为115200；	
	
	
20230727-2240(版本号：00040727_0A030013)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.新增0x002E寄存器，用于控制3GPP IP核的控制S_AXIS_CTRL_tdata接口；		
	
20230801-2150(版本号：00040801_0A030014)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.新增蜂鸣器PL控制，增加0x002F、0x0030寄存器进行蜂鸣器控制；
	
20230807-1130(版本号：00040807_0A030015)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.新增STO模块，用于更准确的定位单帧数据起始点的位置，提升速度；
	2.droneid功能正常，能识别到无人机，STO模块工作正常，待进一步验证；
	
20230809-1210(版本号：00040809_0A030016)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.修改STO模块输入数据I/Q反问题；	


20230814-2100(版本号：00040814_0A030017)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.优化STO模块代码，解决中心位置1和201的问题；
	2.UART0-UART1:修改成偶校验；
	3.Turbo解码模块从BD模块移到顶层；
	4.Droneid功能正常；
	

20230816-2030(版本号：00040816_0A030018)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.新增EMIO[71:56]；RSV_1V8_IO、RSV_3V3_IO  16个GPIO;
	2.Droneid功能正常；


20230817-2200(版本号：00040817_0A030019)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.优化STO模块，解决1和200位置点问题;
	2.Droneid功能正常；

20230818-1200(版本号：00040818_0A030020)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.优化STO模块，加法模块由DSP48资源修改成FF、LUT资源;
	2.修改风扇控制模块，周期由10K修改成25K，10档；
	3.Droneid功能正常；

20230831-1540(版本号：00040831_0A030021)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.修改写DDR协议格式，添加帧头、帧尾、CRC校验等内容；优化写使能和第一个计数器值不为0问题，写ddr调试核；
	2.修复19980点写DDR不正常问题；
	3.Droneid功能正常；

20230902-1240(版本号：00040902_0A030022)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.解决当前帧数据中，包尾STO值是上一帧STO结果问题；
	2.调整帧尾协议格式，修改包尾CRC校验的计算范围，对1-19979个点进行CRC校验；
	3.Droneid功能正常；

20230905-1030(版本号：00040905_0A030023)-LVDS版本：------droneid+parrot+频谱侦测 5EV 平台识别功能版本
	1.新增EMIO[78:72]，信号为RF_GPIO[7:1]；
	2.Droneid功能正常；
