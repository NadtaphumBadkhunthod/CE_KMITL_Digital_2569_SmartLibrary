# esp_upload_top : USB-UART <-> ESP8266 pass-through (EDGE Spartan-7 XC7S15-FTGB196)
set_property -dict { PACKAGE_PIN G1  IOSTANDARD LVCMOS33 } [get_ports {usb_rx}]
set_property -dict { PACKAGE_PIN F2  IOSTANDARD LVCMOS33 } [get_ports {usb_tx}]
set_property -dict { PACKAGE_PIN A10 IOSTANDARD LVCMOS33 } [get_ports {esp_tx}]
set_property -dict { PACKAGE_PIN A12 IOSTANDARD LVCMOS33 } [get_ports {esp_rx}]
set_property -dict { PACKAGE_PIN M4  IOSTANDARD LVCMOS33 } [get_ports {led_rx}]
set_property -dict { PACKAGE_PIN L5  IOSTANDARD LVCMOS33 } [get_ports {led_tx}]
set_property CFGBVS VCCO [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
