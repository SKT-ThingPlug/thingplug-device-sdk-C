#ifndef __W5500HARDWAREDRIVER_H_
#define __W5500HARDWAREDRIVER_H_

#ifdef __cplusplus
 extern "C" {
#endif
   
#include "stm32f4xx_hal.h"

#define W5500_RESET_TIME	1 //ms
extern SPI_HandleTypeDef hspi1W5500;

void W5500HardwareInitilize(void);
void W5500Initialze(void);
void W5500HardwareReset(void);
void W5500WriteByte(unsigned char byte);
unsigned char W5500ReadByte(void);
void W5500Select(void);
void W5500DeSelect(void);
unsigned char wizchip_read(void);
void  wizchip_write(unsigned char wb);

/*for os Mutex*/
void W5500MutexEnter(void);
void W5500MutexExit(void);

#ifdef __cplusplus
}
#endif
#endif /*__W5500HARDWAREDRIVER_H_ */
