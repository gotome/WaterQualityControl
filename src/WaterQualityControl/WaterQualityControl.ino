//****************************************
//                LIBRARYS
//****************************************
//arduino
#include <Arduino.h>
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

//global variables
uint8_t u8Temperature;
uint16_t u16ADC_Raw;
uint16_t u16ADC_Voltage;
uint16_t u16DO;

//oxygen Sensor functions
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

//temperature sensor functions
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup()
{
  //begin serial communication
  Serial.begin(115200);
  //display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  display.clearDisplay();
  //temperature sensor
  sensors.begin();
  //delay time
  delay(10);
}
 
void loop()
{
  sensors.requestTemperatures(); //(uint8_t)READ_TEMP;
  u8Temperature = (uint8_t)sensors.getTempCByIndex(0);
  u16ADC_Raw = analogRead(DO_PIN);
  u16ADC_Voltage = uint32_t(VREF) * u16ADC_Raw / ADC_RES;
 
  Serial.print("Temperaturet:\t" + String(u8Temperature) + "\t");
  Serial.print("ADC RAW:\t" + String(u16ADC_Raw) + "\t");
  Serial.print("ADC Voltage:\t" + String(u16ADC_Voltage) + "\t");
  Serial.println("DO:\t" + String(readDO(u16ADC_Voltage, u8Temperature)) + "\t");
 
  display.clearDisplay();
  display.setCursor(10, 0); //oled display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Dissolved Oxygen");
  
  display.setCursor(30, 20); //oled display
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print((readDO(u16ADC_Voltage, u8Temperature))/1000);
  display.setTextSize(1);
  display.print(" mg/L");
  display.display();
  delay(1000);
}
