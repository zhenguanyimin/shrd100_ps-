# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: F:\shrd100_ps\dev_embed_droneid\01_code\shrd100_app_system\_ide\scripts\debugger_shrd100_app-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source F:\shrd100_ps\dev_embed_droneid\01_code\shrd100_app_system\_ide\scripts\debugger_shrd100_app-default.tcl
# 
connect -url tcp:127.0.0.1:3121
source E:/Xilinx/Vitis/2022.2/scripts/vitis/util/zynqmp_utils.tcl
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent JTAG-SMT2 210251A08870" && level==0 && jtag_device_ctx=="jsn-JTAG-SMT2-210251A08870-04720093-0"}
fpga -file F:/shrd100_ps/dev_embed_droneid/01_code/hw_spec/droneid_PL_0A030023.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw F:/shrd100_ps/dev_embed_droneid/01_code/shrd100_plt/export/shrd100_plt/hw/droneid_PL_0A030023.xsa -mem-ranges [list {0x80000000 0xbfffffff} {0x400000000 0x5ffffffff} {0x1000000000 0x7fffffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
set mode [expr [mrd -value 0xFF5E0200] & 0xf]
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow F:/shrd100_ps/dev_embed_droneid/01_code/shrd100_plt/export/shrd100_plt/sw/shrd100_plt/boot/fsbl.elf
set bp_21_17_fsbl_bp [bpadd -addr &XFsbl_Exit]
con -block -timeout 60
bpremove $bp_21_17_fsbl_bp
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow F:/shrd100_ps/dev_embed_droneid/01_code/shrd100_app/Debug/shrd100_app.elf
configparams force-mem-access 0
bpadd -addr &main
