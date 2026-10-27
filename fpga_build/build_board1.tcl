catch {config_webtalk -install off}
read_vhdl "C:/Users/saire/Desktop/KMITL/Digital/Project/board1.vhd"
read_xdc "board1_top.xdc"
synth_design -top board1_top -part xc7s15ftgb196-1 -directive RuntimeOptimized
catch {
  foreach bc [get_cells -quiet -hier -filter {REF_NAME =~ BUFG*}] {
    set n [get_nets -quiet -of_objects [get_pins -quiet $bc/I]]
    if {[llength $n]} { set_property CLOCK_DEDICATED_ROUTE FALSE $n }
  }
}
place_design -directive Quick
route_design -directive Quick
write_bitstream -force "board1_top.bit"
