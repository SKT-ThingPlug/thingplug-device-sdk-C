#ifndef _SMA_H_
#define _SMA_H_

typedef enum _sma_sensor {
    MOTION_IDX,
    TEMP_IDX,
    HUMI_IDX,
    LIGHT_IDX,
    SENSOR_IDX_MAX
} SMA_SENSOR_IDX;

void SMAGetData(char* sensor, char** output);
int SMAGetSensorIndex(char *sensorType);

#endif//_SMA_H_
