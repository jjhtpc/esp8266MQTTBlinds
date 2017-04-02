#include <Servo.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <PubSubClient.h>


/************ WIFI and MQTT INFORMATION (CHANGE THESE FOR YOUR SETUP) ******************/
#define wifi_ssid "<Your SSID>" //enter your WIFI SSID
#define wifi_password "<Your Password>" //enter your WIFI Password

#define mqtt_server "<Your IP>" // Enter your MQTT server adderss or IP. I use my DuckDNS adddress (yourname.duckdns.org) in this field
#define mqtt_user "<Your User id>" //enter your MQTT username
#define mqtt_password "<Your Password>" //enter your password








WiFiClient espClient;
PubSubClient client(espClient);

Servo myservo;

int val;
int itsatrap = 0;


void setup() {
  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883); //CHANGE PORT HERE IF NEEDED
  client.setCallback(callback);

}


void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);
    String mytopic(topic);
    if (itsatrap == 0 && mytopic == "<Your Payload Topic>" && message.equals("ON")){  
      myservo.attach(D4);
      delay(500);
      myservo.write(90); 
      client.publish("<Your State Topic>", "ON");
      delay(1000);
      myservo.detach();
      }
    else if (mytopic == "<Your Payload Topic>" && message.equalsIgnoreCase("OFF")){
      myservo.attach(D4);
      delay(500);
      myservo.write(0);  
      client.publish("<Your State Topic>", "OFF");
      delay(1000);
      myservo.detach();
    }
    else if (mytopic == "<Your Brightness Topic>"){
      myservo.attach(D4);
      delay(500);
      val = message.toInt(); //converts command to integer to be used for positional arrangement
      val = map (val, 0, 99, 0, 180);
      myservo.write(val);
      client.publish("<Your State Topic>", "ON");
      delay(3000);
      myservo.detach();
      itsatrap = 1;
    }
    else{
        itsatrap = 0;
    }

}



void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
  if (client.connect("ESPBlindstl", mqtt_user, mqtt_password)) {
      Serial.println("connected");

      client.subscribe("<Your Payload Topic>");
      client.subscribe("<Your Brightness Topic>");
      client.publish("<Your State Topic>", "OFF");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



