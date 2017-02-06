#include "Board.h"

/*Include: Header file*/
#include "W5500HardwareDriver.h"

/*Include: W5500 Library*/
#include "wizchip_conf.h"

/*include: Standard IO library*/
#include <stdio.h>

SPI_HandleTypeDef hspi1W5500;

void W5500HardwareInitilize(void)
{
  __GPIOH_CLK_ENABLE();
  /*W5500 CS/INT/RST Clock enable*/
  __GPIOB_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  
  /*W5500 GPIO&SPI1 Clock enable*/
  __GPIOA_CLK_ENABLE();
  
  /*Initialize GPIO Structure*/
  GPIO_InitTypeDef	GPIO_InitStructure;

  /*Initialize CS Pin*/
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Pin = W5500_CS_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(W5500_CS_PORT,&GPIO_InitStructure);
  
  /*Initialize INT Pin*/
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Pin = W5500_INT_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(W5500_INT_PORT,&GPIO_InitStructure);

  /*Ethernet shield does not have individual reset pin*/
  /* ==> Remove R23 and connect RSTN - D9 pin
        Now D9 is Reset pin.*/
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Pin = W5500_RESET_PIN;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(W5500_RESET_PORT,&GPIO_InitStructure);
  
  /*SPI init*/
  hspi1W5500.Instance = W5500_SPI;
  hspi1W5500.Init.Mode = SPI_MODE_MASTER;
  hspi1W5500.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1W5500.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1W5500.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1W5500.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1W5500.Init.NSS = SPI_NSS_SOFT;
  hspi1W5500.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1W5500.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1W5500.Init.TIMode = SPI_TIMODE_DISABLED;
  hspi1W5500.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  hspi1W5500.Init.CRCPolynomial = 10;
  HAL_SPI_Init(&hspi1W5500);
  
  W5500HardwareReset();
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* Peripheral clock enable */
    __SPI1_CLK_ENABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = W5500_CLK_PIN|W5500_MISO_PIN|W5500_MOSI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(W5500_SPI_GPIO_PORT, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  }
  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
} 

void W5500HardwareReset(void)
{
  uint32_t tempTick;
  HAL_GPIO_WritePin(W5500_RESET_PORT,W5500_RESET_PIN,GPIO_PIN_RESET);
  tempTick = HAL_GetTick();
  while((HAL_GetTick() - tempTick) < W5500_RESET_TIME){/*1 ms Delay: Do nothing*/}
  HAL_GPIO_WritePin(W5500_RESET_PORT,W5500_RESET_PIN,GPIO_PIN_SET);
}

void W5500WriteByte(unsigned char txByte)
{
  unsigned char rtnByte;
  while (HAL_SPI_GetState(&hspi1W5500) != HAL_SPI_STATE_READY);
  HAL_SPI_TransmitReceive(&hspi1W5500,&txByte,&rtnByte,1,10);
}

unsigned char W5500ReadByte(void)
{
  unsigned char txByte = 0xff;//dummy
  unsigned char rtnByte;
  while (HAL_SPI_GetState(&hspi1W5500) != HAL_SPI_STATE_READY);
  HAL_SPI_TransmitReceive(&hspi1W5500,&txByte,&rtnByte,1,10);
  return rtnByte;
}

void W5500Select(void)
{
  HAL_GPIO_WritePin(W5500_CS_PORT,W5500_CS_PIN,GPIO_PIN_RESET);
}

void W5500DeSelect(void)
{
  HAL_GPIO_WritePin(W5500_CS_PORT,W5500_CS_PIN,GPIO_PIN_SET);
}
