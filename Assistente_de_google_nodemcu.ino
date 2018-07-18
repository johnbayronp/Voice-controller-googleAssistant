
//https://github.com/adafruit/Adafruit_MQTT_Library <----- Libreria del server MQTT
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WIFI_SSID "BAYRON PEREZ"//Red wifi de house
#define WIFI_PASS "xxxxxx" // Red Wifi del house

#define WIFI_SSID2 "profilelighting1" //  red wifi del trabajo
#define WIFI_PASS2 "xxxxx"// Red wifi del trabajo

#define MQTT_SERV "io.adafruit.com" // Servidor donde alojamos nuestros datos 
#define MQTT_PORT 1883 // Puerto del servidor 
#define MQTT_NAME "johnbayronp" // Nombre y ID del servidor io.adafruit
#define MQTT_PASS "9d5bec628b51426584d3df32c1ce2dde"
#define LED D0 // Posicion de salida de la tarjeta NodeMCU para feeds -> tv 
#define LED2 D3 // Posicion de salida de la tarjeta NodeMCU para feeds -> ventilador

WiFiClient client; //Recibimos el cliente
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS); //asignamos el cliente , servidor, puerto, nombre , contraseña

Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/tv"); // los feeds creados en el servidor io.adafruit Pasamos el cliente(x,direccion);

Adafruit_MQTT_Subscribe onoff2 = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/ventilador"); // los feeds creados en el servidor io.adafruit

// Setup, Conectaremos ala red wifi con MQTT Server
void setup()
{
  Serial.begin(9600);

  //Conectar a WiFi
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

  //Suscribir los topicos creados onoff y onff2
  mqtt.subscribe(&onoff);
  mqtt.subscribe(&onoff2);

  pinMode(LED, OUTPUT); // configuramos las salidas de nuestros pines en este caso de la nodemcu D0, D3
  pinMode(LED2, OUTPUT);
}
void loop()
{
  //Conectar y reconectar a MQTT
  MQTT_connect();

  //Leemos las suscripciones , para que los feeds se ejecuten correctamente
  //Esperamos 5 minutos para que la suscripcion se actualize
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
  
    // Si son correctas las suscripciones y actualizamos.
    if (subscription == &onoff || subscription ==&onoff2)
    {
      //Imprimimos el nuevo valor en nuestra consola 
      Serial.print("onoff: ");
      Serial.println((char*) onoff.lastread); 

      

      Serial.print("onoff2: ");
      Serial.println((char*) onoff2.lastread);
     
      //Leemos los valores para dar acciones 
      if (!strcmp((char*) onoff.lastread, "1")) // valor que asignamos en los applets de IFTTT (Puede resivir char)
      {
        //TV APAGADO
        digitalWrite(LED, LOW);
      } 
      else if(!strcmp((char*) onoff.lastread, "0"))
      {
        digitalWrite(LED, HIGH); 
      }

      
      // VENTILADOR
      if(!strcmp((char*) onoff2.lastread, "3"))
      {
        digitalWrite(LED2, LOW);
      }else if(!strcmp((char*) onoff2.lastread, "on")){
        digitalWrite(LED2, HIGH);
      }
    }
  }
}

//conectamos el servidor MQTT
void MQTT_connect() 
{
  int8_t ret;
  // Paramos la conexion, si esta conectado.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // reconectamos mientras retorne el valor 0.
  { 
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // Esperemos 5 segundos
    retries--;
    if (retries == 0) 
    {
      // Basicamente muere y esperamos que resetee
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}