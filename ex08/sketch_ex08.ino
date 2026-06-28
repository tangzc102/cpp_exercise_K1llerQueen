#include <WiFi.h>
#include <WebServer.h>

// AP热点配置
const char* ap_ssid = "ESP32-LAB54102";
const char* ap_pass = "1234567891";

// 硬件引脚定义
#define TOUCH_PIN 4
#define ALARM_LED 2
#define THRESHOLD 400
#define DEBOUNCE_MS 50

// 系统全局状态变量
bool isArm = false;       // 布防状态：true布防，false撤防
bool isAlarmTrigger = false; // 是否触发报警
bool lastTouchState = false;
unsigned long lastTouchTime = 0;

WebServer server(80);

// 网页页面：布防/撤防按钮
String htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<title>物联网安防报警器</title>
<style>
*{box-sizing:border-box}
body{text-align:center;margin-top:100px;font-size:24px;background:#f0f0f0;}
button{width:180px;height:60px;font-size:22px;margin:20px;padding:10px;cursor:pointer;}
#armBtn{background:#2e7d32;color:white;border:none;border-radius:8px;}
#disarmBtn{background:#c62828;color:white;border:none;border-radius:8px;}
.status{margin-top:40px;font-size:26px;font-weight:bold;}
.arm{color:#2e7d32}
.alarm{color:#c62828}
.normal{color:#555}
</style>
</head>
<body>
<h2>ESP32 安防报警主机</h2>
<button id="armBtn">布防 ARM</button>
<button id="disarmBtn">撤防 DISARM</button>
<div class="status" id="showStatus">当前状态：未布防</div>

<script>
const armBtn = document.getElementById("armBtn");
const disarmBtn = document.getElementById("disarmBtn");
const statusText = document.getElementById("showStatus");

// 布防按钮
armBtn.onclick = ()=>{
  fetch("/arm");
  statusText.innerText = "当前状态：已布防";
  statusText.className = "status arm";
}
// 撤防按钮
disarmBtn.onclick = ()=>{
  fetch("/disarm");
  statusText.innerText = "当前状态：未布防";
  statusText.className = "status normal";
}
// 定时刷新报警状态
setInterval(()=>{
  fetch("/getstate").then(res=>res.text()).then(txt=>{
    if(txt=="alarm"){
      statusText.innerText = "当前状态：报警触发！";
      statusText.className = "status alarm";
    }
  })
},300);
</script>
</body>
</html>
)HTML";

// 主页路由
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// 布防接口
void handleArm() {
  isArm = true;
  server.send(200, "text/plain", "armed");
}

// 撤防接口，同时清除报警
void handleDisarm() {
  isArm = false;
  isAlarmTrigger = false;
  server.send(200, "text/plain", "disarmed");
}

// 获取报警状态接口，前端轮询显示报警
void handleGetState() {
  if(isAlarmTrigger){
    server.send(200, "text/plain", "alarm");
  }else{
    server.send(200, "text/plain", "safe");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ALARM_LED, OUTPUT);
  digitalWrite(ALARM_LED, LOW);

  // 开启AP热点
  WiFi.softAP(ap_ssid, ap_pass);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("热点IP：");
  Serial.println(apIP);

  // 注册路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.on("/getstate", handleGetState);
  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long now = millis();
  int touchValue = touchRead(TOUCH_PIN);
  bool currentTouch = (touchValue < THRESHOLD);

  // 1. 边缘检测+软件防抖，仅布防时才检测入侵
  if(isArm && currentTouch && !lastTouchState){
    if(now - lastTouchTime > DEBOUNCE_MS){
      lastTouchTime = now;
      isAlarmTrigger = true; // 锁定报警，无法自动解除
    }
  }
  lastTouchState = currentTouch;

  // 2. 报警逻辑：报警触发则高频闪烁LED
  if(isAlarmTrigger){
    digitalWrite(ALARM_LED, HIGH);
    delay(80);
    digitalWrite(ALARM_LED, LOW);
    delay(80);
  }else{
    digitalWrite(ALARM_LED, LOW);
  }
}