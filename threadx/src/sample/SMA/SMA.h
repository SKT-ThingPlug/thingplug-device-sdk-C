#ifndef _SMA_H_
#define _SMA_H_

typedef enum _sma_sensor {
  TEMP_IDX = 1, // 0x01
  HUMI_IDX,     // 0x02
  LIGHT_IDX,    //0x03
  MOTION_IDX,   // 0x04
  SENSOR_IDX_MAX
} SMA_SENSOR_IDX;

void SMAGetData(char* sensor, char** output);
int SMAGetSensorIndex(char *sensorType);

#endif//_SMA_H_
