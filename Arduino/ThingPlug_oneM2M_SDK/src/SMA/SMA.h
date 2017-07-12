#ifndef _SMA_H_
#define _SMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#define USE_REAL_SENSOR
#define USE_LCD

#ifdef USE_LCD
void SMAInitLCD(int x, int y);
void SMASetLCDRGB(int R, int G, int B);
void SMASetLCD(int x, int y,char *str, int clear_flag);
#endif

#ifdef USE_REAL_SENSOR
#define TEMP_PIN  0
#define LIGHT_PIN 3
#define PROX_PIN 8
#define LED_PIN 4
#endif


typedef enum _sma_sensor {
  TEMP_IDX = 1, // 0x01
  HUMI_IDX,     // 0x02
  LIGHT_IDX,    //0x03
  MOTION_IDX,   // 0x04
  PROX_IDX,   // 0x05
  SENSOR_IDX_MAX
} SMA_SENSOR_IDX;

void SMAGetData(char* sensor, char** output);
int SMAGetSensorIndex(char *sensorType);
void SMASetLED(int pin, int flag);

#ifdef __cplusplus
}
#endif

#endif//_SMA_H_
