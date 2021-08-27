//****************************************
//                LIBRARYS
//****************************************
//general
#include <stdlib.h>
//arduino
#include <Arduino.h>
#include <math.h>
//communication
#include <SPI.h>
#include <Wire.h>
//display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>
//project
#include <WaterQualityControl.h>
 
//****************************************
//                TABLES
//****************************************
const uint16_t DO_Table[41] = {
  14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
  11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
  9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
  7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410
};

//****************************************
//                GLOBALS
//****************************************
float floatTemperature;
uint16_t u16ADC_Raw;
uint16_t u16ADC_Voltage;
uint16_t u16DO;

//****************************************
//         FUNCTION DECLARATIONS
//****************************************
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//****************************************
//         FUNCTION DEFINITIONS
//****************************************
int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c)
{
#if TWO_POINT_CALIBRATION == 00
  uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#else
  uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#endif
}

float floatPrecision(float n, float i)
{
    return floor(pow(10,i)*n)/pow(10,i);
}

void oledShowStats(void) {
    display.clearDisplay();
    oledPrintOxigen(u16ADC_Voltage, floatTemperature);
    oledPrintTemperature(floatTemperature);
    display.display();
}

void oledPrintOxigen(uint16_t u16ADC_Voltage, float floatTemperature) {
    display.setCursor(10, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.print("Dissolved Oxygen");  
    display.setCursor(30, 10);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.print((readDO(u16ADC_Voltage, (uint8_t)floatTemperature))/1000);
    display.setTextSize(1);
    display.print(" mg/L");
}

void oledPrintTemperature(float floatTemperature) {
    char charTemperature[4]; 
    dtostrf((double)floatTemperature, 4, 2, charTemperature);

    display.setCursor(10, 30);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.print("Current Temperature");
    display.setCursor(30, 40);
    display.setTextSize(2);
    display.print(charTemperature);
    display.setTextSize(1);
    display.print(" ");
    display.print((char)247); //DEGREE SYMBOL
    display.print("C");
}

//setup system and bus communication
void setup()
{
  //begin serial communication
  Serial.begin(115200);
  //display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){ //initialize with the I2C addr 0x3C (128x64)
    Serial.println("Display (SSD1306) allocation failed");
  } 
  display.clearDisplay();
  //temperature sensor
  sensors.begin();
  //pin mode setup 
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  //delay time
  delay(10);
}

//****************************************
//                M A I N
//****************************************
void loop()
{
  sensors.requestTemperatures(); //(uint8_t)READ_TEMP;
  floatTemperature = floatPrecision(sensors.getTempCByIndex(0), (float)2.0) ;
  u16ADC_Raw = analogRead(DO_PIN);
  u16ADC_Voltage = uint32_t(VREF) * u16ADC_Raw / ADC_RES;
  Serial.print("Temperature:\t" + String(floatTemperature, (unsigned char)2) + "\t");
  Serial.print("ADC RAW:\t" + String(u16ADC_Raw) + "\t");
  Serial.print("ADC Voltage:\t" + String(u16ADC_Voltage) + "\t");
  Serial.println("DO:\t" + String((float)(readDO(u16ADC_Voltage, floatTemperature)/(uint16_t)1000), (unsigned char)2) + "\t");
 
  //oled part
  oledShowStats();

  if (floatTemperature > (float)23) {
    digitalWrite(8, LOW);
  } 
  else 
  {    
    digitalWrite(8, HIGH);
  }

  //delay
  delay(1000);
}
