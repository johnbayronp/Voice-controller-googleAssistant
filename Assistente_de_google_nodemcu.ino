
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "BAYRON PEREZ"//Red wifi de house
#define WIFI_PASS "VILLEGAS" // Red Wifi del house

#define WIFI_SSID2 "profilelighting1" //  red wifi del trabajo
#define WIFI_PASS2 "luces2017"// Red wifi del trabajo

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "johnbayronp"
#define MQTT_PASS "9d5bec628b51426584d3df32c1ce2dde"
#define LED D0
#define LED2 D3

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/tv");

Adafruit_MQTT_Subscribe onoff2 = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/ventilador");
//in the setup, we'll connect to the WiFi and the MQTT server:

void setup()
{
  Serial.begin(9600);

  //Connect to WiFi
  Serial.println("\n\nConectando Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(WIFI_SSID,WIFI_PASS);
          if(WiFi.status() != WL_CONNECTED){
            delay(500);
          }
    delay(500);
    Serial.println("Conectando a la red " WIFI_SSID);
  }
  
  Serial.println("Conectado a la red! ");

  //Subscribe to the onoff topic
  mqtt.subscribe(&onoff);
  mqtt.subscribe(&onoff2);

  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
}
void loop()
{
  //Connect/Reconnect to MQTT
  MQTT_connect();

  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
     
    //If we're in here, a subscription updated...
    if (subscription == &onoff || subscription ==&onoff2)
    {
      //Print the new value to the serial monitor
      Serial.print("onoff: ");
      Serial.println((char*) onoff.lastread);

      

      Serial.print("onoff2: ");
      Serial.println((char*) onoff2.lastread);
      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off. Afuera Outside 
     
      if (!strcmp((char*) onoff.lastread, "1"))
      {
        //active low logic
        digitalWrite(LED, LOW);
      } 
      else if(!strcmp((char*) onoff.lastread, "0"))
      {
        digitalWrite(LED, HIGH); 
      }

      
      // Living room - cuarto 
      if(!strcmp((char*) onoff2.lastread, "3"))
      {
        digitalWrite(LED2, LOW);
      }else if(!strcmp((char*) onoff2.lastread, "on")){
        digitalWrite(LED2, HIGH);
      }
    }
  }
}

void MQTT_connect() 
{
  int8_t ret;
  // Stop if already connected
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) 
    {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
  // ping the server to keep the mqtt connection alive<br>  if (!mqtt.ping())<br>  {<br>    mqtt.disconnect();<br>  }<br>}
