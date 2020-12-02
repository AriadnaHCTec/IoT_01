/*

Programa principal de interacción entre usuario y estación mensajera para guardar mediciones en base de datos relacional de sql.

Por Luis Ignacio Ferro Salinas A01378248
    Ari

Última modificación: 1 de diciembre de 2020

*/

// Preprocesamiento de oximetría
#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
MAX30105 particleSensor; // Instancia de sensor.
uint32_t irBuffer[100]; // Para guardar del sensor infrarrojo.
uint32_t redBuffer[100]; // Guardar información del sensor rojo.
int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

// Preprocesamiento para temperatura.
#include "DHT.h"
#define DHTTYPE DHT11
#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE); 

//Internet
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
const char *red = "FerSal";//Ari: INFINITUMC99E  
                                   // Red Luis Ferro: FerSal
const char *password = "0037605980";//Ari: FEw3Cp4M2j
String urlBase = "http://192.168.0.108/IoT";  // GET? Ari: 192.168.1.90
                                                             // Ip interna de compu Luis Ferro: 192.168.0.108
                                                             // La ip pública de Ari receptora es 189.225.66.113
HTTPClient http;
WiFiClient clienteWiFi;
 
void setup() {
  // Para depurar por el puerto serial.
  abrirSerial();
  // Conectarse a la red.
  conectarARed();
  // Inicialización de temperatura.
  dht.begin();
  // Inicializar sensor de oximetría.
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)){
    Serial.println(F("No se encontró el sensor de oximetría"));
    while(1); // Terminar.
  }
  // Setup de sensor.
  byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384  
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
}

void loop(){
  String claveUsuario;

  Serial.println("\nBienvenido usuario\n");
  bool usuarioNuevo = pregunta("Hola buen usuario, es nuevo?");
  
  if (usuarioNuevo){
    // query para obtener clave de nuevo usuario
    claveUsuario = obtenerClaveMaximaUsuario();
    int claveUsuarioInt = claveUsuario.toInt();
    claveUsuarioInt++;
    claveUsuario = String(claveUsuarioInt);
  } else{
      Serial.println("\nEscriba su número de usuario en el puerto serial.\n");
      while(Serial.available() == 0){
        claveUsuario = String(Serial.read());  
      }
  }
  
  // Hacer y enviar las medidas que el usuario quiera.
  bool quiereMedirTemperatura = pregunta("¿Quiere medir su temperatura?");
  if (quiereMedirTemperatura){
    medicionTemperatura(claveUsuario);
  }

  bool quiereMedirOximetriaFrecuencia = pregunta("¿Quiere medir su oximetría y frecuencia cardíaca?");
  if (quiereMedirOximetriaFrecuencia){
    medicionOximetriaFrecuenciaCardiaca(claveUsuario);
  }
  
  delay(100000);
}

String obtenerClaveMaximaUsuario(){
  // El argumento que representa tabla debe ir sin la palabra Medición.
  String url = String(urlBase + "/consultaClaveMaxima.php";
  Serial.println(url);
  // Enviarlos por la red al servicio
  // Solicitar la conexión al servicio
  if ( http.begin(clienteWiFi, url) ){ 
    int codigo = http.GET();  // Realizar petición
    Serial.printf("Código: %d\n", codigo);
      if (codigo == HTTP_CODE_OK || codigo == HTTP_CODE_MOVED_PERMANENTLY){
        String claveUsuario = http.getString();  // Respuesta
        Serial.println(claveUsuario);
        return claveUsuario;
      } else{
      Serial.printf("GET falló, error: %s\n", http.errorToString(codigo).c_str());
    }
  } else{
    Serial.println("No es posible hacer la conexión");
  }
  http.end();
}


//http://192.168.1.90/IoT/insertaMedicionFrecuenciaCardiaca.php?frecuenciaCardiaca=10&claveUsuario=1&modelo=MAX30102
void enviarDatos(String tabla, float promedioMedicion, String claveUsuario, String modelo){
  // El argumento que representa tabla debe ir sin la palabra Medición.
  String url = String( urlBase + "/insertaMedicion" + tabla + ".php?" + decapitalize(tabla) + "=" + String(promedioMedicion) + "&claveUsuario=" + claveUsuario + "&modelo=" + modelo);
  Serial.println(url);
  // Enviarlos por la red al servicio
  // Solicitar la conexión al servicio
  if ( http.begin(clienteWiFi, url) ){ 
    int codigo = http.GET();  // Realizar petición
    Serial.printf("Código: %d\n", codigo);
      if (codigo == HTTP_CODE_OK || codigo == HTTP_CODE_MOVED_PERMANENTLY){
        String respuesta = http.getString();  // Respuesta
        Serial.println(respuesta);
      } else{
      Serial.printf("GET falló, error: %s\n", http.errorToString(codigo).c_str());
    }
  } else{
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

void medicionTemperatura(String claveUsuario){
  // Lectura de temperatura y humedad.
  float temperaturas[100];
  float promedioTemperatura = 0;
  for (int i = 1; i <= 99; i++){
    temperaturas[i] = dht.readTemperature();
    if(i>89){
      promedioTemperatura += temperaturas[i];
    }
  }
  promedioTemperatura /= 10;
  enviarDatos("TemperaturaCorporal", promedioTemperatura, claveUsuario, "DHT11");
}

void medicionOximetriaFrecuenciaCardiaca(String claveUsuario){
  // Esperar indicación de usuario para comenzar a medir.
  Serial.println("\nColoque su sensor de oximetría contra su dedo firmemente. Cuando este listo mande byte a puerto serial.\n");
  while(Serial.available() == 0){
    Serial.read();  
  }
  bufferLength = 100;
  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data
      
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample
    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.println(irBuffer[i], DEC);
  }
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  for (byte i = 25; i < 100; i++){
    redBuffer[i - 25] = redBuffer[i];
    irBuffer[i - 25] = irBuffer[i];
  }
  for (byte i = 75; i < 100; i++){
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample
    Serial.print(F(", HR="));
    Serial.print(heartRate, DEC);

    Serial.print(F(", HRvalid="));
    Serial.print(validHeartRate, DEC);

    Serial.print(F(", SPO2="));
    Serial.print(spo2, DEC);

    Serial.print(F(", SPO2Valid="));
    Serial.println(validSPO2, DEC);
    //send samples and calculation result to terminal program through UART
  }
  //After gathering 25 new samples recalculate HR and SP02
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  Serial.println(String(validSPO2));
  Serial.println(String(validHeartRate));
  enviarDatos("Oximetria", spo2, claveUsuario, "MAX30102");
  enviarDatos("FrecuenciaCardiaca", heartRate, claveUsuario, "MAX30102");
}

bool pregunta(String textoPregunta){
  /* textoPregunta string de máximo 32 caracteres.
   *  boton 1 y 2 - son referencias a los botones 
   *  */
 bool esperando = false;
 byte boton1 = false;
 byte boton2 = false;
 Serial.println(textoPregunta);
 delay(5000);
 while(true){
  boton1 = digitalRead(D5);
  boton2 = digitalRead(D6);
  if (boton1 && not boton2){
    Serial.println("Ha respondido sí.");
    delay(2000);
    return true;
  }else if (not boton1 && boton2){
    Serial.println("Ha respondido no.");
    delay(2000);
    return false;
  }else{
    if (!esperando){
      Serial.println("Esperando...");
    }
  } 
  delay(10); //Para solucionar error de reset
 }
}


  
