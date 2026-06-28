#include <WiFi.h>
#include <WebServer.h>

// AP热点配置
const char* ap_ssid = "ESP32-LAB54102";
const char* ap_pass = "1234567891"; // 至少8位
const int LED_PIN = 2; // D2 GPIO2

// PWM参数
const int PWM_FREQ = 5000;
const int PWM_RES  = 8;

WebServer server(80);
int ledBright = 0;

// 网页：滑动条 + JS fetch异步调光
String htmlPage = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<title>ESP32 AP 网页无极调光</title>
<style>
body {text-align:center;font-size:22px;margin-top:80px;background:#f5f5f5;}
#slider {width:80%;height:30px;margin:30px 0;}
span {font-weight:bold;color:#d32f2f;font-size:28px;}
</style>
</head>
<body>
<h2>LED亮度滑动调节器</h2>
<input type="range" id="slider" min="0" max="255" value="0">
<p>当前亮度值：<span id="valShow">0</span></p>

<script>
const slider = document.getElementById("slider");
const show = document.getElementById("valShow");

slider.oninput = function(){
    let v = this.value;
    show.innerText = v;
    // 异步GET请求发送亮度数值
    fetch(`/led?val=${v}`);
}
</script>
</body>
</html>
)HTML";

// 主页路由
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

// 调光接口，解析url参数val
void handleLed() {
  if (server.hasArg("val")) {
    ledBright = server.arg("val").toInt();
    // 限制0~255范围
    ledBright = constrain(ledBright, 0, 255);
    ledcWrite(LED_PIN, ledBright);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  
  // PWM初始化LED
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RES);
  ledcWrite(LED_PIN, 0);

  // 开启AP热点
  WiFi.softAP(ap_ssid, ap_pass);
  IPAddress apIP = WiFi.softAPIP();
  Serial.println("ESP32热点已开启");
  Serial.print("热点名称：");
  Serial.println(ap_ssid);
  Serial.print("访问IP：");
  Serial.println(apIP);

  // 注册网页路由
  server.on("/", handleRoot);
  server.on("/led", handleLed);
  server.begin();
}

void loop() {
  server.handleClient();
}