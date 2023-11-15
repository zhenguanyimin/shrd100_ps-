set VITIS_WORKSPACE [lindex $argv 0]
set PLATFORM_SPR shrd100_plt/platform.spr
set XSA  hw_spec/aeag_top.xsa
puts "platform read $PLATFORM_SPR"
platform read $PLATFORM_SPR
puts "platform active"
platform active {shrd100_plt}
puts "platform config -updatehw  $XSA"
platform config -updatehw  $XSA
puts "platform generate"
platform generate
puts "setws $VITIS_WORKSPACE"
setws $VITIS_WORKSPACE
puts "app build -name shrd100_app"
app build -name shrd100_app
puts "build finish"