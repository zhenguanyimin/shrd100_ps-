反制枪无人机型识别算法代码修改说明
2023.2.7

utils.c
Line120：修正卷积边缘位置；

droneSniffer.c
修改图传特征库的多个pulseWErr参数
注释飞控特征库参数，nLib = 8；

downLoadSniffer.h
修改卷积核Mask，改为5x5的平滑窗

downLoadSniffer.c
Line425：检测阈值最大迭代次数改为20

反制枪无人机型识别算法代码修改说明
2023.1.17

droneSniffer.c
1. 暂时删除Mini2机型，对应NLib = 6；
2. 修改Mavic3机型的参数（最低脉冲数）。

downLoadSniffer.c
3. 增加两行代码（注释时间20230117），去除频带边缘检测造成的额外类型误报。


