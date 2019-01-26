#include <BME280I2C.h>
#include "Wire.h"
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ModbusTCPSlave.h>
#include <Ticker.h>

////////////////////////////////////////////////////////
#define SSID            "WiFi"
#define SSIDPWD         "P@ssw0rdWiFi"
//WIFI Settings
//byte ip[]      = { 192, 168, 1, 126};
//byte gateway[] = { 192, 168, 1, 1 };
//byte subnet[]  = { 255, 255, 255, 0 };
////////////////////////////////////////////////////////

Ticker ExModbus;
Ticker LCD_Pr;
ModbusTCPSlave Mb;
BME280I2C bme;
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

uint8_t gradus[8] = {0x6,0x9,0x9,0x6,0x0,0x0,0x0}; 

float T,P,H;    // for data of BME280


void setup()
{
  Serial.begin(115200);
  // Интерфейс I2C подключен ко 2 (SDA) и 14 (SCL) пинам (не стандартно).Пин SCB к 3.3 вольта 
  Wire.begin(2,14);   // SCK - GPIO14, SDA - GPIO2

  Mb.begin(SSID, SSIDPWD);
//  Mb.begin(SSID, SSIDPWD, ip, gateway, subnet);
  delay(100);

  lcd.begin(2,14);      // In ESP8266-01, SDA=0, SCL=2               
  lcd.backlight();
  lcd.createChar(1, gradus);
  if(!bme.begin()){
    lcd.setCursor(0,0);
    lcd.print("Error BME280 sensor!!!");
    while(1);
  }
}

void loop()
{
  Mb.Run();
  delay(10);
  //Ticker Function
  ExModbus.attach_ms(25,ExchangeModbus);
  LCD_Pr.attach(2,Print_Lcd);
}

void ExchangeModbus(){
  Mb.MBHoldingRegister[0] = T*100;
  Mb.MBHoldingRegister[1] = P*10;
  Mb.MBHoldingRegister[2] = H*100;
}

void Print_Lcd(){
  
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_mmHg);
  bme.read(P, T, H, tempUnit, presUnit);

  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("T=");
  lcd.setCursor(2,0);
  lcd.print(T);
  lcd.setCursor(6,0);
  lcd.write(byte(1));
  lcd.setCursor(7,0);
  lcd.print("C");

  lcd.setCursor(9,0);
  lcd.print("P=");
  lcd.setCursor(11,0);
  lcd.print(P);

  lcd.setCursor(0,1);
  lcd.print("H=");
  lcd.setCursor(2,1);
  lcd.print(H);
  lcd.setCursor(6,1);
  lcd.print(" %");

  Serial.println(T);
  Serial.println(H);
  Serial.println(P);
  Serial.println();
}



