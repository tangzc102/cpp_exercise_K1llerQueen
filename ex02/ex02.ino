// 定义LED引脚，ESP32通常板载LED连接在GPIO 2
const int ledPin = 2;
// 毫秒计时变量必须用 unsigned long
unsigned long previousTime = 0;
// 1Hz：亮1s灭1s，翻转间隔1000ms
const unsigned long interval = 1000;
bool ledState = LOW;

void setup() {
  // 初始化串口通信，设置波特率为115200
  Serial.begin(115200);
  // 将LED引脚设置为输出模式
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentTime = millis();
  // 判断是否到达翻转时间
  if (currentTime - previousTime >= interval)
  {
    previousTime = currentTime;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    
    Serial.print("系统运行毫秒数：");
    Serial.print(currentTime);
    Serial.print(" | LED当前状态：");
    Serial.println(ledState ? "点亮" : "熄灭");
  }
}
// 作业2：使用millis函数实现无阻塞1HzLED闪烁，不使用delay阻塞程序

