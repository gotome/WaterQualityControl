#pragma once 
//***************************************************************
//                      DEFINE MACROS
//***************************************************************

// DISPLAY
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define DO_PIN A1
 
// OXIGEN SENSOR
#define VREF 5000    //VREF (mv)
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
#define ONE_WIRE_BUS 5

//***************************************************************
//                     FUNCTION PROTOTYPES
//***************************************************************
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c); 