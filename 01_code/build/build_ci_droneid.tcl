puts "setws $::env(VITIS_WORKSPACE)"
setws $::env(VITIS_WORKSPACE)

set VERSION [lindex $argv 0]

importprojects shrd100_plt

puts "platform active"
platform active {shrd100_plt}

puts "platform generate"
platform generate

importprojects shrd100_app
puts "app build -name shrd100_app"
app config -name shrd100_app -remove define-compiler-symbols WORKMODE=DRONEID
app config -name shrd100_app define-compiler-symbols WORKMODE=1
app config -name shrd100_app define-compiler-symbols EMBED_SOFTWARE_PS_VERSION_STR=\\\"$VERSION\\\"
app build -name shrd100_app
