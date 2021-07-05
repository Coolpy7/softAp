// #include <WiFi.h> //esp32
#include <ESP8266WiFi.h> //esp8266
#include <WiFiUdp.h> //引用以使用UDP
#include <ArduinoJson.h>

const char *ssid = "esp32";
const char *password = "";

WiFiUDP Udp;                      //创建UDP对象
unsigned int localUdpPort = 2333; //本地端口号

void setup()
{
  Serial.begin(115200);
  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  Udp.begin(localUdpPort); //启用UDP监听以接收数据
}

void loop()
{
  int packetSize = Udp.parsePacket(); //获取当前队首数据包长度
  if (packetSize)                     //如果有数据可用
  {
    char buf[packetSize];
    Udp.read(buf, packetSize); //读取当前包数据

    Serial.print("From IP: ");
    Serial.println(Udp.remoteIP());
    Serial.print("From Port: ");
    Serial.println(Udp.remotePort());

    DynamicJsonDocument doc(64);
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, buf);
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      return;
    }
    const char* rssid = doc["ssid"];
    Serial.println(String(rssid));
    const char* rpassword = doc["password"];
    Serial.println(String(rpassword));

    DynamicJsonDocument outDoc(64);
    outDoc["code"] = 0;
    String output;
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); //准备发送数据
    serializeJson(outDoc, Udp);
    Udp.println();
    Udp.endPacket();
  }
}
