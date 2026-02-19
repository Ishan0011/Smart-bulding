#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define PIRPIN 3
#define RELAY1 7
#define RELAY2 8

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial espSerial(10, 11); // espSerial(RX, TX) - Arduino RX, TX

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  dht.begin();
  lcd.init(); lcd.backlight();
  pinMode(PIRPIN, INPUT);
  pinMode(RELAY1, OUTPUT); digitalWrite(RELAY1, HIGH);
  pinMode(RELAY2, OUTPUT); digitalWrite(RELAY2, HIGH);
}

void loop() {
  float t = dht.readTemperature();
  int motion = digitalRead(PIRPIN);
  bool occupied = (motion == HIGH);

  // simple control logic
  if (occupied) digitalWrite(RELAY1, LOW); else digitalWrite(RELAY1, HIGH);
  if (occupied && t > 30) digitalWrite(RELAY2, LOW); else digitalWrite(RELAY2, HIGH);

  // LED Display
  lcd.setCursor(0,0);
  lcd.print("T:"); lcd.print(t); lcd.print("C ");
  lcd.setCursor(0,1);
  lcd.print(occupied ? "Motion: Y " : "Motion: N ");
  lcd.print(t>30?"Fan:On":"Fan:Off");

  // Send data to NodeMCU for cloud (CSV or JSON)
  String payload = String(t) + "," + String(occupied ? 1 : 0);
  espSerial.println(payload); // NodeMCU must read from its Serial and post

  delay(2000);
}
