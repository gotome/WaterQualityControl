#pragma once 
//***************************************************************
//                      TYPEDEFINITIONS
//***************************************************************

//***************************************************************
//                      DEFINE MACROS
//***************************************************************
// DISPLAY
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SUB_MENUE_1 (int)0       // value for oled sub menue 1
#define SUB_MENUE_2 (int)1       // value for oled sub menue 2

//ANALOG PINS
#define DO_PIN_A0 A0
#define DO_PIN_A1 A1
 
// General Constant for analog input
#define VREF 5000    //VREF (mv)
#define VREF_2 5      //VREF (V)
#define ADC_RES 1024 //ADC Resolution
 
//Single-point calibration Mode=0
//Two-point calibration Mode=1
#define TWO_POINT_CALIBRATION 1 
#define READ_TEMP (25) //Current water temperature ℃, Or temperature sensor function
 
//Single point calibration needs to be filled CAL1_V and CAL1_T
#define CAL1_V (131) //mv
#define CAL1_T (25)   //℃
//Two-point calibration needs to be filled CAL2_V and CAL2_T
//CAL1 High temperature point, CAL2 Low temperature point
#define CAL2_V (1300) //mv
#define CAL2_T (15)   //℃

//TEMPERATURE SENSOR
#define ONE_WIRE_BUS 2

//***************************************************************
//                     FUNCTION PROTOTYPES
//***************************************************************
double readDO(uint32_t voltage_mv, uint8_t temperature_c); 
void qualityControl(double dTemperature, double dDO);
double doublePrecision(double n, double i);
void oledShowStats(double dDO, double dTemperature, double dBatVoltage);
void switchOnRelay(uint8_t pin);
void switchOffRelay(uint8_t pin);
void oledPrintBatteryVoltage(double batVoltage);
void oledSwitchMenue();