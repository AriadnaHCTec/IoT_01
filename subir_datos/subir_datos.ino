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
String urlBase = "http://192.168.1.90/IoT/insertaMedicion";  // GET? Ari: 192.168.1.90
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
  Serial.println(sizeof(temperaturas));
  for (int i = 1; i <= 99; i++){
    temperaturas[i] = dht.readTemperature();
    humedades[i] = dht.readHumidity();
    if(i>89){
      promedioTemperatura += temperaturas[i];
    }
    delay(10);
  }
  /*for (int i = 89; i <= 99; i++){
    promedioTemperatura += temperaturas[i];
    //promedioHumedad += humedades[i];
    delay(10);
  }*/
  promedioTemperatura /= 10;
  promedioHumedad /= 10;
  enviarDatos("TemperaturaCorporal", promedioTemperatura,"1", "DHT11");
  
  delay(1000000); // Cada 10 segundos

}

//http://192.168.1.90/IoT/insertaMedicionFrecuenciaCardiaca.php?frecuenciaCardiaca=10&claveUsuario=1&modelo=MAX30102
void enviarDatos(String tabla,float promedioMedicion ,String id, String modelo){
  String url = String( urlBase + tabla+".php?"+decapitalize(tabla)+"="+String(promedioMedicion)+"&claveUsuario="+id+"&modelo="+modelo);

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

String decapitalize(String palabra){
  String final = String(palabra[0]);
  final.toLowerCase();
  return final+palabra.substring(1);
}
