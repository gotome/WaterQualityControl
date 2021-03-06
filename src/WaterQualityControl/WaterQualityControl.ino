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
const double DO_Table[41] = {
  14.460, 14.220, 13.820, 13.440, 13.090, 12.740, 12.420, 12.110, 11.810, 11.530,
  11.260, 11.010, 10.770, 10.530, 10.300, 10.080, 9.860, 9.660, 9.460, 9.270,
  9.080, 8.900, 8.730, 8.570, 8.410, 8.250, 8.110, 7.960, 7.820, 7.690,
  7.560, 7.430, 7.300, 7.180, 7.070, 6.950, 6.840, 6.730, 6.630, 6.530, 6.410
};

//****************************************
//                GLOBALS
//****************************************
uint16_t u16ADC_Raw; //raw value oxygen sensor
uint16_t u16ADC_Voltage; //voltage oxygen sensor
uint16_t u16ADC_RawBat; //raw value battery
uint16_t u16ADC_VoltageBat; //voltage battery
uint16_t u16DO;
int intCurrSubMenueVal = SUB_MENUE_1;
int intOldSubMenueVal = SUB_MENUE_1;

//****************************************
//         FUNCTION DECLARATIONS
//****************************************
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//****************************************
//         FUNCTION DEFINITIONS
//****************************************
double readDO(uint32_t voltage_mv, uint8_t temperature_c)
{
#if TWO_POINT_CALIBRATION == 00
  uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
  return (double)(voltage_mv * DO_Table[temperature_c] / V_saturation);
#else
  uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
  return (double)(voltage_mv * DO_Table[temperature_c] / V_saturation);
#endif
}

double doublePrecision(double n, double i)
{
    return floor(pow(10,i)*n)/pow(10,i);
}

void oledShowStats(double dDO, double dTemperature, double dBatVoltage) {    
    display.clearDisplay();
    switch (intCurrSubMenueVal)
    {
    case SUB_MENUE_1:
      oledPrintOxigen(dDO);
      oledPrintTemperature(dTemperature);
      break;    
    case SUB_MENUE_2:
      oledPrintBatteryVoltage(dBatVoltage);
      break;    
    default:
      //nothing to do
      break;
    }
    display.display();
}

void oledPrintBatteryVoltage(double batVoltage) {  
    char charVal[5]; 
    dtostrf(batVoltage, 4, 2, charVal);

    display.setCursor(10, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Battery Voltage");  
    display.setCursor(30, 10);
    display.setTextSize(2);
    display.print(charVal);
    display.setTextSize(1);
    display.print(" V");
}

void oledSwitchMenue() {
  int bMenueBtn = digitalRead(PIN7);
  if (bMenueBtn == HIGH) {    
    if (intCurrSubMenueVal == intOldSubMenueVal) {
      intCurrSubMenueVal = (intCurrSubMenueVal == SUB_MENUE_1) ? SUB_MENUE_2 : SUB_MENUE_1;
    }
  } 
  else {
    intOldSubMenueVal = intCurrSubMenueVal;
  }
}

void oledPrintOxigen(double dDO) {  
    char charVal[5]; 
    dtostrf(dDO, 4, 1, charVal);

    display.setCursor(10, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Dissolved Oxygen");  
    display.setCursor(30, 10);
    display.setTextSize(2);
    display.print(charVal);
    display.setTextSize(1);
    display.print(" mg/L");
}

void oledPrintTemperature(double dTemperature) {
    char charVal[5]; 
    dtostrf(dTemperature, 4, 1, charVal);

    display.setCursor(10, 30);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Current Temperature");
    display.setCursor(30, 40);
    display.setTextSize(2);
    display.print(charVal);
    display.setTextSize(1);
    display.print(" ");
    display.print((char)247); //DEGREE SYMBOL
    display.print("C");
}

void switchOnRelay(uint8_t pin) {
  //low voltage switching relay!
  digitalWrite(pin, LOW); 
}

void switchOffRelay(uint8_t pin) {
  //low voltage switching relay!
  digitalWrite(pin, HIGH); 
}

void qualityControl(double dTemperature, double dDO) {
    if (dDO < (double)5) {
    switchOnRelay(8);
  } 
  else 
  {    
    switchOffRelay(8);
  }
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
  pinMode(PIN7, INPUT);
  //delay time
  delay(10);
}

//****************************************
//                M A I N
//****************************************
void loop()
{
  //write globals
  sensors.requestTemperatures(); //(uint8_t)READ_TEMP;
  u16ADC_Raw = analogRead(DO_PIN_A1);
  u16ADC_Voltage = uint32_t(VREF) * u16ADC_Raw / ADC_RES;
  u16ADC_RawBat = analogRead(DO_PIN_A0);
  u16ADC_VoltageBat = uint32_t(VREF) * u16ADC_RawBat / ADC_RES;
  //write locals
  double dTemperature = doublePrecision((double)sensors.getTempCByIndex(0), (double)2.0);
  double dDO = doublePrecision(readDO(u16ADC_Voltage, dTemperature), (double)2.0);
  double dBatVoltage = doublePrecision((double)(((double)16/(double)5) * u16ADC_VoltageBat / (double)1000.0), (double)2.0);

  //serial monitor output
  // Serial.print("Temperature:\t" + String(dTemperature, (unsigned char)2) + "\t");
  // Serial.print("ADC RAW:\t" + String(u16ADC_Raw) + "\t");
  // Serial.print("ADC Voltage:\t" + String(u16ADC_Voltage) + "\t");
  // Serial.println("DO:\t" + String(dDO, (unsigned char)2) + "\t");

  //oled part  
  oledSwitchMenue();
  oledShowStats(dDO, dTemperature, dBatVoltage);
  //water quality control
  qualityControl(dTemperature, dDO); 

  //delay
  delay(100);
}
