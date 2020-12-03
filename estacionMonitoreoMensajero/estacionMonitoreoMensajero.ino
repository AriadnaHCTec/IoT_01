/*

Programa principal de interacción entre usuario y estación mensajera para guardar mediciones en base de datos relacional de sql.

Por Luis Ignacio Ferro Salinas A01378248
    Ariadna Huesca Coronado A01749161

Última modificación: 1 de diciembre de 2020

*/



// Preprocesamiento de pantalla lcd.
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

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
String urlBase = "http://189.225.127.57/IoT";  // GET? Ari: 192.168.1.90
                                                             // Ip interna de compu Luis Ferro: 192.168.0.108
                                                             // La ip pública de Ari receptora es 189.225.127.57
HTTPClient http;
WiFiClient clienteWiFi;


// Preprocesamiento de mqtt.
#include <PubSubClient.h>
PubSubClient client(clienteWiFi);
long lastMsg = 0;

const char* mqtt_server = "broker.hivemq.com";
const char* channelTopic = "/medicion/temperatura";
const char* channelTopic2 = "/medicion/oximetria";
const char* channelTopic3 = "/medicion/frecuencia";

void setup(){
  // Para depurar por el puerto serial.
  abrirSerial();
  // Inicialización de pantalla lcd.
  lcd.init();
  lcd.backlight();
  // Conectarse a la red.
  conectarARed();
  // Inicialización de temperatura.
  dht.begin();
  // Inicializar sensor de oximetría.
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)){
    Serial.println(F("No se encontró el sensor de oximetría"));
    imprimePantalla("No sensor :(");
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

  // botones
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);

  // mqtt setup
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop(){

  Serial.println("\nBienvenido usuario a la estación de monitoreo mensajera Monitoreo es salud\n");
  imprimePantalla("Bienvenido");
  delay(5000);

  String claveUsuario = obtenerClaveUsuario();
  
  // Hacer y enviar las medidas que el usuario quiera.
  bool quiereMedirTemperatura = pregunta("Quiere medir su temperatura?");
  if (quiereMedirTemperatura){
    bool temperaturaTiempoReal = pregunta("temperatura tiempo real?");
    if (temperaturaTiempoReal){
      medicionTemperatura(claveUsuario, true);
    } else{
      medicionTemperatura(claveUsuario, false);
    }
  }

  bool quiereMedirOximetriaFrecuencia = pregunta("Medir oximetria y frecuencia?");
  if (quiereMedirOximetriaFrecuencia){
    bool oximetriaFrecuenciaTiempoReal = pregunta("frecuencia, oxi. tiempo real?");
    if (oximetriaFrecuenciaTiempoReal){
      medicionOximetriaFrecuenciaCardiaca(claveUsuario, true);
    } else{
      medicionOximetriaFrecuenciaCardiaca(claveUsuario, false);
    }
  }
  
  delay(100000);
}

void callback(char* topic, byte* payload, unsigned int length){
  char *cstring = (char *) payload;
  cstring[length] = '\0';    // Agrega un caracter de terminación
  Serial.println(cstring);
  if (cstring[1]=='R') {
  Serial.println("\tRojo");
  digitalWrite(LED_BUILTIN,HIGH);
  }else if (cstring[1]=='G') {
  Serial.println("\tVerde");
  digitalWrite(LED_BUILTIN,LOW);
  } else if (cstring[1]=='B') {
  Serial.println("\tAzul");
  digitalWrite(LED_BUILTIN,LOW);
  }
} //termina callback 

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()){
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
   //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str())){
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      client.subscribe(channelTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //termina reconnect()

void imprimePantalla(String texto){
  lcd.clear();
  lcd.setCursor(0, 0);
  if (texto.length() > 15){
    String parteUno = texto.substring(0, 15);
    lcd.print(parteUno);
    lcd.setCursor(0, 1);
    String parteDos = texto.substring(16, 31);
    lcd.print(parteDos);
  } else{
    lcd.print(texto);
  }
}

String obtenerClaveUsuario(){
  String claveUsuario;
  bool usuarioNuevo = pregunta("Hola buen usuario, es nuevo?");
  if (usuarioNuevo){
    // query para obtener clave de nuevo usuario
    claveUsuario = obtenerClaveMaximaUsuario();
    int claveUsuarioInt = claveUsuario.toInt();
    claveUsuarioInt++;
    claveUsuario = String(claveUsuarioInt);
    Serial.println("\Su clave de usuario será: ");
    Serial.println(claveUsuario);
    imprimePantalla("Su clave sera: ");
    delay(5000);
    imprimePantalla(claveUsuario);
    delay(5000);
    Serial.println("\nNo la olvide\n");
    imprimePantalla("No la olvide");
    delay(5000);
    Serial.println("Por favor diríjase a la liga: y registre sus datos en el formulario. Cuando esté listo, envíe algo por el puerto serial.");
    imprimePantalla("LLene formulario");
    while (Serial.available() == 0){
      Serial.read();      
    }
    Serial.print("Ya se insertó como usuario");
    imprimePantalla("formulario listo");
    delay(5000);
  } else{
      Serial.println("\nEscriba su número de usuario en el puerto serial.\n");
      imprimePantalla("Escriba clave");
      while (1){
        if (Serial.available() > 0){
          claveUsuario = Serial.readString();
          break;
        }
      }
      claveUsuario.replace("\n", "");
      Serial.println("\nHa confirmado que su número de usuario es ");
      Serial.println(claveUsuario);
      imprimePantalla("Su clave es");
      delay(5000);
      imprimePantalla(claveUsuario);
      delay(5000);
  }
  return claveUsuario;
}


String obtenerClaveMaximaUsuario(){
  String url = String(urlBase + "/consultaClaveMaxima.php");
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
  imprimePantalla("Se han guardado los datos");
  delay(5000);
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

void medicionTemperatura(String claveUsuario, bool mqtt){
  // Lectura de temperatura y humedad.
  float temperaturas[100];
  float promedioTemperatura = 0;
  Serial.println("\n Mande señal al puerto serial cuando este listo");
  imprimePantalla("mande señal cuando este listo");
  while (Serial.available() == 0){
      Serial.read();
  }
  for (int i = 1; i <= 99; i++){
    temperaturas[i] = dht.readTemperature();
    imprimePantalla("Leyendo temperatura");
    if(i>89){
      promedioTemperatura += temperaturas[i];
    }
  }
  promedioTemperatura /= 10;
  if (mqtt){
    if (!client.connected()){
        reconnect();
    }
    client.loop();
    client.publish(channelTopic, String(promedioTemperatura).c_str());
  }  
  enviarDatos("TemperaturaCorporal", promedioTemperatura, claveUsuario, "DHT11");
}

void medicionOximetriaFrecuenciaCardiaca(String claveUsuario, bool mqtt){
  // Esperar indicación de usuario para comenzar a medir.
  Serial.println("\nColoque su sensor de oximetría contra su dedo firmemente. Cuando este listo mande byte a puerto serial.\n");
  imprimePantalla("mande señal cuando este listo");
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
    imprimePantalla("midiendo oximetria y frecuencia");
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
    imprimePantalla("midiendo oximetria y frecuencia");
    //send samples and calculation result to terminal program through UART
  }
  //After gathering 25 new samples recalculate HR and SP02
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  if (mqtt){
    if (!client.connected()){
        reconnect();
    }
    client.loop();
    client.publish(channelTopic2, String(spo2).c_str());
    client.publish(channelTopic3, String(heartRate).c_str());
  }
  delay(5000);
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
 imprimePantalla(textoPregunta);
 delay(5000);
 while(true){
  boton1 = digitalRead(D5);
  boton2 = digitalRead(D6);
  if (boton1 && not boton2){
    Serial.println("\nHa respondido sí.");
    imprimePantalla("Si");
    delay(5000);
    return true;
  } else if (not boton1 && boton2){
    Serial.println("\nHa respondido no.");
    imprimePantalla("No");
    delay(5000);
    return false;
  } else{
    if (!esperando){
      Serial.println("Esperando");
      imprimePantalla("Esperando");
      esperando = true;
    } else{
      Serial.print(".");
      imprimePantalla("Esperando respuesta");
    }
  } 
  delay(10); //Para solucionar error de reset
 }
}


  
