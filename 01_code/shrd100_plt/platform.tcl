# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct D:\project\shrd100_ps\dev_embed\code\shrd100_plt\platform.tcl
# 
# OR launch xsct and run below command.
# source D:\project\shrd100_ps\dev_embed\code\shrd100_plt\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform active {shrd100_plt}
platform config -updatehw {F:/shrd100_ps/test/01_code/hw_spec/Aeag_top.xsa}
platform clean
platform generate




platform generate -domains freertos10_xilinx_domain,freertos10_xilinx_psu_cortexa53_1,zynqmp_fsbl,zynqmp_pmufw 
platform clean

platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_detect/01_code/hw_spec/Aeag_top.xsa}
platform generate
platform config -updatehw {D:/project/shrd100_ps/dev_embed_detect/01_code/hw_spec/STEP3_bit/Aeag_top.xsa}
platform generate -domains zynqmp_pmufw 
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_detect/01_code/hw_spec/Aeag_top.xsa}
platform clean
platform generate
platform clean
platform generate
platform generate
platform generate
platform generate -domains zynqmp_fsbl,zynqmp_pmufw 
platform generate
platform generate -domains zynqmp_fsbl,zynqmp_pmufw 
platform generate -domains zynqmp_fsbl 
platform generate
platform generate -domains zynqmp_fsbl,zynqmp_pmufw 
platform generate
platform generate
platform generate -domains zynqmp_fsbl,zynqmp_pmufw 
platform active {shrd100_plt}
domain active {freertos10_xilinx_domain}
bsp reload
bsp reload
domain active {zynqmp_pmufw}
bsp reload
bsp reload
platform generate -domains 
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_detect/01_code/hw_spec/Aeag_top.xsa}
platform clean
platform generate
platform generate -domains freertos10_xilinx_domain,freertos10_xilinx_psu_cortexa53_1,zynqmp_fsbl,zynqmp_pmufw 
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_detect_5ev/code_1/hw_spec/Aeag_top.xsa}
platform generate
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_detect_5ev/code_1/hw_spec/Aeag_top.xsa}
platform clean
platform generate
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_detect_5ev/code_1/hw_spec/Aeag_top.xsa}
platform generate -domains 
platform generate -domains zynqmp_pmufw 
platform generate -domains zynqmp_pmufw 
platform generate -domains zynqmp_pmufw 
platform generate -domains zynqmp_pmufw 
platform generate
platform active {shrd100_plt}
platform generate
bsp reload
platform generate
platform generate -domains freertos10_xilinx_domain,zynqmp_pmufw 
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_5ev/01_code/hw_spec/Aeag_top.xsa}
platform generate -domains 
platform config -updatehw {D:/project/shrd100_ps/dev_embed_5ev/01_code/hw_spec/Aeag_top.xsa}
platform generate -domains 
platform config -updatehw {D:/project/shrd100_ps/dev_embed_5ev/01_code/hw_spec/Aeag_top.xsa}
platform generate -domains 
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/test/dev_embed_5ev/01_code/hw_spec/Aeag_top.xsa}
platform generate -domains freertos10_xilinx_domain,freertos10_xilinx_psu_cortexa53_1,zynqmp_pmufw 
platform config -updatehw {D:/project/shrd100_ps/test/dev_embed_5ev/01_code/hw_spec/v217/Aeag_top.xsa}
platform generate -domains 
platform config -updatehw {D:/project/shrd100_ps/test/dev_embed_5ev/01_code/hw_spec/v214/Aeag_top.xsa}
platform generate -domains 
platform generate
platform generate
platform generate -domains zynqmp_pmufw 
platform generate -domains zynqmp_pmufw 
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_5ev/01_code/hw_spec/Aeag_top.xsa}
platform generate -domains zynqmp_pmufw 
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/bit/cfg_droneid_5ev_1254_20230726_1630/Aeag_top.xsa}
platform clean
platform generate
domain active {freertos10_xilinx_domain}
bsp reload
bsp config stdin "psu_uart_0"
bsp config stdout "psu_uart_0"
bsp write
bsp reload
catch {bsp regenerate}
platform generate -domains freertos10_xilinx_domain 
platform clean
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_5ev_dir/01_code/hw_spec/cfg_spectrum_5ev_1254_20230726_1630/Aeag_top.xsa}
platform generate
platform generate -domains zynqmp_fsbl,zynqmp_pmufw 
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_5ev_dir/01_code/hw_spec/cfg_droneid_5ev_00040727_0A030013/Aeag_top.xsa}
platform generate -domains zynqmp_fsbl 
platform generate -domains zynqmp_fsbl 
platform config -updatehw {D:/project/shrd100_ps/dev_embed_5ev_dir/01_code/hw_spec/cfg_droneid_5ev_00040728_0B030012/Aeag_top.xsa}
platform generate -domains zynqmp_fsbl 
platform active {shrd100_plt}
platform generate -domains 
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_5ev_dir/01_code/hw_spec/droneid_PL_0B030012/droneid_PL_0B030012.xsa}
platform generate
platform generate -domains zynqmp_fsbl 
platform generate -domains zynqmp_fsbl 
platform generate -domains zynqmp_fsbl 
platform clean
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_doneid/01_code/hw_spec/droneid_PL_0A030014/droneid_PL_0A030014.xsa}
platform generate
platform clean
platform generate
platform active {shrd100_plt}
platform generate -domains 
platform generate
platform generate
platform active {shrd100_plt}
platform config -updatehw {D:/project/shrd100_ps/dev_embed_doneid/01_code/hw_spec/droneid_PL_0A030018/droneid_PL_0A030018.xsa}
platform clean
platform generate
platform clean
platform clean
platform active {shrd100_plt}
platform active {shrd100_plt}
platform config -updatehw {F:/shrd100_ps/dev_embed_droneid/01_code/hw_spec/droneid_PL_0A030018/droneid_PL_0A030018.xsa}
platform generate
platform generate -domains freertos10_xilinx_domain,zynqmp_pmufw 
platform generate
platform active {shrd100_plt}
platform generate -domains 
platform clean
platform config -updatehw {F:/shrd100_ps/dev_embed_droneid/01_code/hw_spec/droneid_PL_0A030023.xsa}
platform config -updatehw {F:/shrd100_ps/dev_embed_droneid/01_code/hw_spec/droneid_PL_0A030023.xsa}
platform generate
platform clean
platform generate
platform generate -domains freertos10_xilinx_domain,zynqmp_fsbl 
platform generate -domains freertos10_xilinx_domain,zynqmp_fsbl 
platform generate
platform generate
platform generate -domains freertos10_xilinx_domain,zynqmp_fsbl,zynqmp_pmufw 
platform generate -domains freertos10_xilinx_domain,zynqmp_pmufw 
platform generate -domains zynqmp_pmufw 
platform clean
