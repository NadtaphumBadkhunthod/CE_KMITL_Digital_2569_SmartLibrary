catch {config_webtalk -install off}
read_vhdl "C:/Users/saire/Desktop/KMITL/Digital/Project/esp_upload.vhd"
read_xdc "esp_upload_top.xdc"
synth_design -top esp_upload_top -part xc7s15ftgb196-1 -directive RuntimeOptimized
place_design -directive Quick
route_design -directive Quick
write_bitstream -force "esp_upload_top.bit"
