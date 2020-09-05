/*
基本的ESP8266 MQTT示例

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>

int pinDHT11 = 2;                  //IO2
SimpleDHT11 dht11(pinDHT11);

const char* ssid = "XXXXXX";//手机Wi-Fi热点名
const char* password = "XXXXXX";//手机Wi-Fi热点密码
const char* mqtt_server = "192.168.4.1";//服务器地址 可以是域名

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int ledPin = 2; // GPIO2 of ESP8266

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
  if ((char)payload[0] == '1') {          // 通过接收的信息 '1' 来控制开或关
    digitalWrite(ledPin, HIGH); 
      Serial.println("HIGH");
  } else {
    digitalWrite(ledPin, LOW); 
    Serial.println("LOW");
  }
}

void reconnect() {
    String clientId = "ESP-01A-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {         //连接成功
      client.subscribe("开关控制");
      client.publish("开关控制", "0");     // 连接成功发送主题和消息
    } else {
      delay(5000);                                  // Wait 5 seconds before retrying
    }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  
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
  
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");
  delay(1500);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  readTempAndHumidity()
}
