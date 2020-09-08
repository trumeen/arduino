/*
基本的ESP8266 MQTT示例

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>

int pinDHT11 = 2;                  //IO2
SimpleDHT11 dht11(pinDHT11);

const char* ssid = "WeWork";//手机Wi-Fi热点名
const char* password = "P@ssw0rd";//手机Wi-Fi热点密码
const char* mqtt_server = "10.125.30.30";//服务器地址 可以是域名

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int ledPin = 2; // GPIO2 of ESP8266

int lastTemperature = 0;
int lastHumidity = 0;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("接收消息[");Serial.print(topic);Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
    String clientId = "ESP-01A-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {         //连接成功
      Serial.println("MQTT连接成功！！！");
      // client.subscribe("/trumeen/cmd");
      client.publish("/trumeen/home/temperature/state", "ONLINE");     // 连接成功发送主题和消息
    } else {
      delay(5000);                                  // Wait 5 seconds before retrying
    }
}

void setup() {
  pinMode(ledPin, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void readTempAndHumidity(){
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  //  String tmp = "{";            //字符串拼接
  //  tmp += String('"');
  //  tmp += String("temperature");
  //  tmp += String('"');
  //  tmp += String(':');
  //  tmp += String(temperature);
  //  tmp += String(',');
  //  tmp += String('"');
  //  tmp += String("humidity");
  //  tmp += String('"');
  //  tmp += String(":");
  //  tmp += String(humidity);
  //  tmp += String("}");
  // client.publish("/trumeen/publish", tmp.c_str());     // 连接成功发送主题和消息
  
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");
   if((int)temperature!=lastTemperature){
      String temp = "";
      temp+=(int)temperature;
      client.publish("/trumeen/home/temperature/value", temp.c_str());     // 连接成功发送主题和消息
     lastTemperature = (int)temperature;
   }
  
  if((int)humidity!=lastHumidity){
      String temp = "";
      temp+=(int)humidity;
      client.publish("/trumeen/home/humidity/value", temp.c_str());     // 连接成功发送主题和消息
      lastHumidity = (int)humidity;
  }
  delay(1500);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  readTempAndHumidity();
}
