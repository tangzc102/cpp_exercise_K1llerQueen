#include <WiFi.h>
#include <WebServer.h>

// AP热点配置（统一和前面实验一致）
const char* ap_ssid = "ESP32-LAB54102";
const char* ap_pass = "1234567891";

// 触摸硬件定义
#define TOUCH_PIN 4
WebServer server(80);

// 网页仪表盘，JS定时器AJAX拉取触摸数据
String dashHtml = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<title>实时触摸传感器仪表盘</title>
<style>
body{text-align:center;background:#111;color:#fff;margin-top:120px;font-family:Arial;}
h2{font-size:32px;}
#touchVal{font-size:90px;color:#0cf;font-weight:bold;margin:40px 0;}
.tip{font-size:20px;color:#aaa}
</style>
</head>
<body>
<h2>触摸传感器实时数值仪表盘</h2>
<div id="touchVal">0</div>
<p class="tip">手靠近引脚数值变小，离开恢复变大</p>

<script>
// 定时AJAX拉取传感器数据
function getTouchData(){
  fetch("/getTouch")
    .then(res=>res.text())
    .then(val=>{
      document.getElementById("touchVal").innerText = val;
    })
}
// 每100ms刷新一次，实时流畅
setInterval(getTouchData, 100);
</script>
</body>
</html>
)HTML";

// 返回主页仪表盘
void handleRoot(){
  server.send(200, "text/html", dashHtml);
}

// 接口：返回当前触摸原始ADC数值
void handleGetTouch(){
  int rawTouch = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(rawTouch));
}

void setup() {
  Serial.begin(115200);

  // 开启AP热点
  WiFi.softAP(ap_ssid, ap_pass);
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("仪表盘访问地址：");
  Serial.println(apIP);

  // 注册网页与数据接口路由
  server.on("/", handleRoot);
  server.on("/getTouch", handleGetTouch);
  server.begin();
}

void loop() {
  server.handleClient();
}