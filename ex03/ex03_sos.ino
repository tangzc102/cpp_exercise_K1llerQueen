const int ledPin = 2;

// 时序参数 和你之前示例完全匹配
const unsigned long shortOn  = 200;
const unsigned long longOn   = 600;
const unsigned long flashGap = 200;
const unsigned long charGap  = 500;
const unsigned long wordGap  = 2000;

unsigned long prevTime = 0;
int state = 0;   // 0:S  1:O  2:S  3:结束停顿
int flashCnt = 0;// 当前字母闪烁次数

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  unsigned long now = millis();
  unsigned long elapsed = now - prevTime;

  switch(state)
  {
    case 0: // S：3次短闪
      runFlash(3, shortOn);
      break;
    case 1: // O：3次长闪
      runFlash(3, longOn);
      break;
    case 2: // S：3次短闪
      runFlash(3, shortOn);
      break;
    case 3: // 一套SOS结束，长时间熄灭等待
      digitalWrite(ledPin, LOW);
      if(elapsed >= wordGap)
      {
        state = 0;
        prevTime = now;
      }
      break;
  }
}

// 闪烁n次，单次点亮tOn毫秒
void runFlash(int n, unsigned long tOn)
{
  unsigned long now = millis();
  unsigned long elapsed = now - prevTime;

  if(flashCnt < n)
  {
    if(elapsed < tOn)
    {
      digitalWrite(ledPin, HIGH);
    }
    else if(elapsed < tOn + flashGap)
    {
      digitalWrite(ledPin, LOW);
    }
    else
    {
      flashCnt++;
      prevTime = now;
    }
  }
  else
  {
    digitalWrite(ledPin, LOW);
    if(elapsed >= charGap)
    {
      flashCnt = 0;
      state++;
      prevTime = now;
    }
  }
}
// 作业3：millis无阻塞实现SOS灯光
// 时序：3短闪S → 3长闪O → 3短闪S，整套结束停顿2秒循环