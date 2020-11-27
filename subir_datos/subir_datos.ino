/*
Programa para subir mediciones a la base de datos en sql.



*/

// Preprocesamiento para temperatura.
#include "DHT.h"
#define DHTTYPE DHT11
#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 


void setup() {
  // Inicialización de temperatura.
  dht.begin();
  Serial.begin();
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
    promedioHumedad += humedad[i];
  }
  promedioTemperatura /= 10;
  promedioHumedad /= 10;

  
  
  
}
