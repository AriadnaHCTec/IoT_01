void setup() {
  // put your setup code here, to run once:
   pinMode(D1, INPUT);
   pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  byte val = digitalRead(D1);

  if (val == HIGH){
    digitalWrite(LED_BUILTIN, LOW);
  } else{
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
