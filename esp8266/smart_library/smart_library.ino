#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "index_html.h"

const char* AP_SSID = "SmartLibrary";
const char* AP_PASS = "library123";  // อย่างน้อย 8 ตัวอักษร

// zone/shelf: 1 = แดง, 2 = น้ำเงิน, 3 = เขียว (ไฟจริงบนโมเดล ผู้ใช้กำหนด 2026-10-25)
struct Book {
  int id;
  const char* name;
  int zone, shelf, count;
};
// เรียง Book No.1-9 ตามตำแหน่ง: โซน 1 ชั้นแดง/น้ำเงิน/เขียว แล้วโซน 2, โซน 3
Book books[] = {
  { 1, "Book No.1", 1, 1, 3 },
  { 2, "Book No.2", 1, 2, 1 },
  { 3, "Book No.3", 1, 3, 2 },
  { 4, "Book No.4", 2, 1, 0 },
  { 5, "Book No.5", 2, 2, 2 },
  { 6, "Book No.6", 2, 3, 1 },
  { 7, "Book No.7", 3, 1, 4 },
  { 8, "Book No.8", 3, 2, 2 },
  { 9, "Book No.9", 3, 3, 1 },
};
const int NBOOK = sizeof(books) / sizeof(books[0]);

ESP8266WebServer server(80);

// flags จาก FPGA: bit0 ไฟห้อง, bit1 Manual, bit2 พัดลม, bit3 ควันอันตราย, bit4-6 จำนวนโต๊ะที่มีคนนั่ง 0-7
const int SEAT_TOTAL = 7;  // ต้องตรงกับ SEAT_TOTAL ใน board1.vhd
uint8_t flags = 0, acHalf = 50, loc = 0;
int followId = -1;
unsigned long lastFrame = 0;
unsigned long folAt = 0;  // เวลาที่ส่งคำสั่งหาหนังสือ ใช้แยกเฟรมเก่าที่ยังไม่มีตำแหน่งใหม่

void sendCmd(uint8_t cmd, uint8_t arg) {
  uint8_t frame[4] = { 0xA5, cmd, arg, (uint8_t) ~(cmd ^ arg) };
  Serial.write(frame, 4);
}

void readFpga() {
  static uint8_t buf[5];
  static uint8_t n = 0;
  while (Serial.available()) {
    uint8_t b = Serial.read();
    if (n == 0 && b != 0x5A) continue;
    buf[n++] = b;
    if (n == 5) {
      n = 0;
      if (buf[4] == (uint8_t) ~(buf[1] ^ buf[2] ^ buf[3])) {
        flags = buf[1];
        acHalf = buf[2];
        loc = buf[3];
        lastFrame = millis();
      }
    }
  }
}

void handleState() {
  if (loc == 0 && lastFrame > folAt) followId = -1;  // ล้างเฉพาะเมื่อเฟรมใหม่กว่าคำสั่งบอกว่าไม่มีตำแหน่ง
  char json[220];
  snprintf(json, sizeof json,
           "{\"lm\":\"%s\",\"lo\":%d,\"fan\":%d,\"ac\":%d.%d,\"smoke\":%d,"
           "\"seat\":%d,\"seats\":%d,\"fol\":%d,\"online\":%d}",
           (flags & 0x02) ? "manual" : "auto",
           flags & 0x01,
           (flags >> 2) & 1,
           acHalf / 2, (acHalf & 1) ? 5 : 0,
           (flags >> 3) & 1,
           (flags >> 4) & 7, SEAT_TOTAL,
           followId,
           millis() - lastFrame < 1000 ? 1 : 0);
  server.send(200, "application/json", json);
}

void handleSet() {
  if (server.hasArg("lm")) sendCmd(0x01, server.arg("lm") == "manual" ? 1 : 0);
  if (server.hasArg("lo")) sendCmd(0x03, server.arg("lo").toInt() ? 1 : 0);
  if (server.hasArg("fan")) sendCmd(0x04, server.arg("fan").toInt() ? 1 : 0);

  if (server.hasArg("fol")) {
    int id = server.arg("fol").toInt();
    uint8_t target = 0;
    followId = -1;
    for (int i = 0; i < NBOOK; i++) {
      if (books[i].id == id) {
        followId = id;
        target = (books[i].zone << 4) | books[i].shelf;
      }
    }
    sendCmd(0x02, target);
    folAt = millis();
  }

  delay(20);  // รอ FPGA ตอบสถานะล่าสุด
  readFpga();
  handleState();
}

void handleBooks() {  // [[id,"name",zone,shelf,count],...]
  String s = "[";
  for (int i = 0; i < NBOOK; i++) {
    if (i) s += ',';
    s += "[" + String(books[i].id) + ",\"" + books[i].name + "\"," + String(books[i].zone) + "," + String(books[i].shelf) + "," + String(books[i].count) + "]";
  }
  server.send(200, "application/json", s + "]");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS, 1, 0, 8);  // เปิดเว็บที่ http://192.168.4.1

  server.on("/", [] {
    server.send_P(200, "text/html", INDEX_HTML);
  });
  server.on("/api/state", handleState);
  server.on("/api/set", handleSet);
  server.on("/api/books", handleBooks);
  server.begin();
}

void loop() {
  server.handleClient();
  readFpga();

  static unsigned long t = 0;
  if (millis() - t >= 200) {  // ขอสถานะจาก FPGA ทุก 0.2 วินาที
    t = millis();
    sendCmd(0x00, 0x00);
  }
}