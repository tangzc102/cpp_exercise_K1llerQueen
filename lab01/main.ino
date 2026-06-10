// 定义板载 LED 引脚，避免使用 "魔法数字"
#define LED_PIN 2
void setup () {
// 初始化串口通信
Serial.begin (115200);
// 初始化板载 LED 引脚为输出模式
pinMode (LED_PIN, OUTPUT);
}
void loop () {
Serial.println ("Hello ESP32!");
digitalWrite (LED_PIN, HIGH); // 点亮 LED
delay (250); // 等待 0.25 秒
digitalWrite (LED_PIN, LOW); // 熄灭 LED
delay (250); // 等待 0.25 秒
}
