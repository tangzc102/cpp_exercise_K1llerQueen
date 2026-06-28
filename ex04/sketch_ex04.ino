#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 400    // 触摸触发阈值，根据硬件微调
#define DEBOUNCE_TIME 50 // 软件防抖间隔(ms)

// 自锁状态变量
bool ledState = false;
// 触摸历史状态：用于边缘检测
bool lastTouch = false;
// 防抖时间戳
unsigned long lastTouchTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState); // 上电默认熄灭
}

void loop() {
  unsigned long now = millis();
  int touchVal = touchRead(TOUCH_PIN);
  bool currentTouch = (touchVal < THRESHOLD);

  // 1. 边缘检测：上次未触摸，本次触摸（触摸上升沿瞬间）
  if (currentTouch && !lastTouch) {
    // 2. 软件防抖判断
    if (now - lastTouchTime > DEBOUNCE_TIME) {
      ledState = !ledState;         // 翻转LED自锁状态
      digitalWrite(LED_PIN, ledState);
      lastTouchTime = now;
      Serial.print("触摸触发，LED状态：");
      Serial.println(ledState ? "亮" : "灭");
    }
  }

  // 更新上一帧触摸状态，用于下一轮边缘判断
  lastTouch = currentTouch;
  delay(20); // 小幅延时降低CPU占用
}