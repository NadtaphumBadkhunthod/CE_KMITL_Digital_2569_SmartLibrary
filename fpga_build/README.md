# fpga_build

สำเนาไฟล์ขา (XDC) และสคริปต์ build จาก `FPGA_Builder_Package/build/`
ใช้ build ด้วย Vivado โดยรันจากโฟลเดอร์ `FPGA_Builder_Package/build/` (สคริปต์อ่าน XDC แบบ relative และอ่าน VHDL จาก path เต็มของโฟลเดอร์ Project)

```
vivado -mode batch -source build_board1.tcl -nojournal -log vivado_board1.log
```

ถ้าแก้ขาใน `FPGA_Builder_Package/build/*.xdc` ให้คัดลอกมาที่นี่ด้วย
