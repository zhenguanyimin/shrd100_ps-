# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: D:\project\shrd100_ps\test\dev_embed_5ev\01_code\shrd100_app_system\_ide\scripts\systemdebugger_shrd100_app_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source D:\project\shrd100_ps\test\dev_embed_5ev\01_code\shrd100_app_system\_ide\scripts\systemdebugger_shrd100_app_system_standalone.tcl
# 
connect -url tcp:127.0.0.1:3121
source D:/Xilinx/Vitis/2022.2/scripts/vitis/util/zynqmp_utils.tcl
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent JTAG-SMT2 210251A08870" && level==0 && jtag_device_ctx=="jsn-JTAG-SMT2-210251A08870-04720093-0"}
fpga -file D:/project/shrd100_ps/test/dev_embed_5ev/01_code/shrd100_app/_ide/bitstream/Aeag_top.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw D:/project/shrd100_ps/test/dev_embed_5ev/01_code/shrd100_plt/export/shrd100_plt/hw/Aeag_top.xsa -mem-ranges [list {0x80000000 0xbfffffff} {0x400000000 0x5ffffffff} {0x1000000000 0x7fffffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
set mode [expr [mrd -value 0xFF5E0200] & 0xf]
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow D:/project/shrd100_ps/test/dev_embed_5ev/01_code/shrd100_plt/export/shrd100_plt/sw/shrd100_plt/boot/fsbl.elf
set bp_10_55_fsbl_bp [bpadd -addr &XFsbl_Exit]
con -block -timeout 60
bpremove $bp_10_55_fsbl_bp
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow D:/project/shrd100_ps/test/dev_embed_5ev/01_code/shrd100_app/Debug/shrd100_app.elf
targets -set -nocase -filter {name =~ "*A53*#1"}
rst -processor
dow D:/project/shrd100_ps/test/dev_embed_5ev/01_code/det_alg/Debug/det_alg.elf
configparams force-mem-access 0
bpadd -addr &main
