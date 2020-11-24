/* 
 *  
 *  Código de estación de monitoreo mensajero.
 *  
 *  Por Luis Ignacio Ferro Salinas A01378248
 *  
 *  18 de noviembre de 2020.
 *  
*/

// https://github.com/thehookup/Wireless_MQTT_Doorbell/blob/master/GPIO_Limitations_ESP8266_NodeMCU.jpg

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // instancia de patalla V-V a pantalla.

void setup() {
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0); //columns, rows
  lcd.print("Hola");
  lcd.setCursor(0, 1);
  lcd.print("Bienvenido");
  delay(4000);
}

void loop() {
  lcd.print("Bienvenido");
}
  /*int numeroUsuario;
 
  // FASE 1

  int fase1 = pregunta("Ya tiene numero");

  lcd.clear();
  lcd.setCursor(0, 0);
  // Por ahora asumimos que sí hay espacio para nuevo usuario.
  if (fase1 == 0){
    lcd.print("No escogio nada");
    delay(3000);
    while(1){}
  } else if (fase1 == 1){
    lcd.print("Su numero sera");
    lcd.setCursor(0, 1);
    lcd.print("x");
    delay(3000);
  } else if (fase1 == 2){
    lcd.print("ingrese numero");
    lcd.setCursor(0, 1);
    lcd.print("en DIP switch");
    numeroUsuario = leeUsuario();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Su numero sera");
    lcd.setCursor(0, 1);
    lcd.print(numeroUsuario);
    delay(3000);
  }*/
  
  // Leer datos del usuario   
//}


/*int pregunta(String textoPregunta){
  /* textoPregunta string de máximo 32 caracteres.
   *  boton 1 y 2 - son referencias a los botones 
  */
 /* int regresa;
  byte boton1;
  byte boton2;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(textoPregunta);
  
  for (int i = 1; i <= 1000000; i++){
    boton1 = digitalRead(D5);
    boton2 = digitalRead(D6);
    if (boton1){
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Escogio si");
     delay(3000);
     regresa = 2;
     break;
    } else if(boton2){
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Escogio no");
     delay(3000);
     regresa = 1;
     break;
    } else{
     regresa = 0;
    }
  }

  return regresa;
}*/

/*int leeUsuario(){
  int regresa;
  byte bit8;
  for (int i = 1; i <= 1000000; i++){
    bit8 = digitalRead(D0);

    if (bit8){
      regresa = 1;
    } else{
      regresa = 0;
    }
  }
  return regresa;    
}*/
