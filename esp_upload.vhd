-- ============================================================
-- esp_upload_top : สะพาน UART สำหรับอัปโหลดโปรแกรม ESP8266
--                  ผ่านพอร์ต USB ของบอร์ด EDGE Spartan-7
--
--   คอม (USB-UART ของบอร์ด)  <--->  FPGA  <--->  ESP8266 (บนบอร์ด)
--
--   ไม่มี logic อะไรนอกจากต่อสายทะลุ (pass-through) จึงใช้ได้ทุก baud
--   (115200 / 74880 / 921600) และไม่ต้องใช้ clock
--
--   usb_rx  (G1, UART_RXD)  : ข้อมูลจากคอม   -> ส่งต่อไปขา RX ของ ESP (A12)
--   esp_tx  (A10, WIFI_RXD) : ข้อมูลจาก ESP  -> ส่งต่อไปขา TX ของบอร์ด (F2) ให้คอมอ่าน
--   led     (ตัวเลือก)      : กะพริบตามข้อมูลที่วิ่ง เอาไว้ดูว่ามีสัญญาณ
--
--   ทิศของขายืนยันบนบอร์ดแล้ว 2026-10-24 (ลูกศรในคู่มือหน้า 6-7 กลับด้าน)
-- ============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity esp_upload_top is
  port (
    usb_rx : in  std_logic;   -- G1  : คอม -> FPGA
    usb_tx : out std_logic;   -- F2  : FPGA -> คอม
    esp_tx : in  std_logic;   -- A10 : ESP -> FPGA
    esp_rx : out std_logic;   -- A12 : FPGA -> ESP
    led_rx : out std_logic;   -- LED D9 (ตัวเลือก) ติดเมื่อคอมส่งข้อมูล
    led_tx : out std_logic    -- LED D8 (ตัวเลือก) ติดเมื่อ ESP ส่งข้อมูล
  );
end esp_upload_top;

architecture rtl of esp_upload_top is
begin
  esp_rx <= usb_rx;          -- คอม -> ESP  (คำสั่ง/เฟิร์มแวร์จาก esptool)
  usb_tx <= esp_tx;          -- ESP -> คอม  (ข้อความบูต/ตอบกลับ esptool)

  -- สาย UART ว่าง = '1' จึงกลับขั้วให้ LED ติดตอนมีข้อมูล
  led_rx <= not usb_rx;
  led_tx <= not esp_tx;
end rtl;
