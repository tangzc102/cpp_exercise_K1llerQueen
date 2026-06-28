#define TOUCH_PIN 4
#define LED_PIN 2
#define THRESHOLD 400
#define DEBOUNCE_TIME 50   // 触摸防抖ms

// 触摸状态变量（边缘检测用）
bool lastTouch = false;
unsigned long lastTouchTime = 0;

// 呼吸灯档位与参数
int speedLevel = 1;  // 档位 1/2/3
int ledBright = 0;   // PWM亮度 0~255
int fadeDir = 1;     // 1变亮，-1变暗

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, ledBright);
  Serial.println("ex05 多档位触摸呼吸灯启动");
  Serial.println("档位1：慢速 | 档位2：中速 | 档位3：快速");
}

void loop() {
  unsigned long now = millis();
  int touchVal = touchRead(TOUCH_PIN);
  bool currentTouch = (touchVal < THRESHOLD);

  // ========== 触摸边缘检测 + 防抖：切换档位 ==========
  if (currentTouch && !lastTouch) {
    if (now - lastTouchTime > DEBOUNCE_TIME) {
      lastTouchTime = now;
      // 档位循环 1→2→3→1
      speedLevel += 1;
      if (speedLevel > 3) speedLevel = 1;
      Serial.print("切换至档位：");
      Serial.println(speedLevel);
    }
  }
  lastTouch = currentTouch;

  // ========== 根据档位设置呼吸步长 ==========
  int step;
  switch (speedLevel) {
    case 1: step = 1; break;  // 慢速，每次+1/-1
    case 2: step = 3; break;  // 中速
    case 3: step = 6; break;  // 快速
    default: step = 1;
  }

  // ========== PWM 呼吸逻辑 ==========
  ledBright += fadeDir * step;

  // 到达极值反转方向
  if (ledBright >= 255) {
    ledBright = 255;
    fadeDir = -1;
  }
  if (ledBright <= 0) {
    ledBright = 0;
    fadeDir = 1;
  }

  analogWrite(LED_PIN, ledBright);
  delay(10); // 基础小延时，保证呼吸平滑
}