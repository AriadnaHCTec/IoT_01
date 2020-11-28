/*
Programa para subir mediciones a la base de datos en sql.



*/

// Preprocesamiento para temperatura.
#include "DHT.h"
#define DHTTYPE DHT11
#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 

//Internet
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
const char *red = "INFINITUMC99E";//Ari: INFINITUMC99E
const char *password = "FEw3Cp4M2j";//Ari: FEw3Cp4M2j
String urlBase = "http://192.168.1.90/grabar/pruebainserta.php?fecha=2020-11-19&hora=22:05:20&valor=";  // GET?
HTTPClient http;
WiFiClient clienteWiFi;

void setup() {
  // Para depurar por el puerto serial
  abrirSerial();
  // Conectarse a la red
  conectarARed();
  // Inicialización de temperatura.
  dht.begin();
}

void loop() {
  // Lectura de temperatura y humedad.

  float temperaturas[100];
  float humedades[100];
  float promedioTemperatura = 0;
  float promedioHumedad = 0;

  for (int i = 1; i <= sizeof(temperaturas) - 1; i++){
    temperaturas[i] = dht.readTemperature();
    humedades[i] = dht.readHumidity();
  }
  for (int i = 89; i <= 99; i++){
    promedioTemperatura += temperaturas[i];
    promedioHumedad += humedades[i];
  }
  promedioTemperatura /= 10;
  promedioHumedad /= 10;  

  // Armar la ruta del servicio
  String url = String( urlBase + String(promedioTemperatura) );
  Serial.println(url);

  // Enviarlos por la red al servicio
  // Solicitar la conexión al servicio
  if ( http.begin(clienteWiFi, url) ) { 
    int codigo = http.GET();  // Realizar petición
    Serial.printf("Código: %d\n", codigo);
      if (codigo == HTTP_CODE_OK || codigo == HTTP_CODE_MOVED_PERMANENTLY) {
        String respuesta = http.getString();  // Respuesta
        Serial.println(respuesta);
      } else {
      Serial.printf("GET falló, error: %s\n", http.errorToString(codigo).c_str());
    }
  } else {
    Serial.println("No es posible hacer la conexión");
  }
  http.end();
  delay(10000); // Cada 10 segundos

}

void abrirSerial() {
  // Para la consola serial
  Serial.begin(57600);
  delay(1000);
  Serial.println("\n");
}

// Conectarse a la red
void conectarARed() {
  Serial.println("\nConectando a la red");
  WiFi.begin(red, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    // Esperar medio segundo
    delay(500);
    Serial.print(".");
  }
  
  // Ya se conectó, avisar
  Serial.println("\nConectado!");
}
