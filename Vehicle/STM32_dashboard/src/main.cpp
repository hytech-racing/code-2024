#include "main.h"
#include "SPI.h"

// initialize/construct singleton object(this was a nightmare to setup and
// can probably be replaced at some point with traditional class)
hytech_dashboard* hytech_dashboard::_instance = NULL;
// get instance and set to dashboard pointer for easier reference
hytech_dashboard* dashboard = hytech_dashboard::getInstance();

// DO NOT MOVE THIS!!! VVVVVVV has conflict with STM32_CAN for some reason
Dashboard_Controls dash_controls = Dashboard_Controls();

//Create STM32_CAN object to pass to DashboardCAN
STM32_CAN stm_can( CAN2, DEF);
CAN_message_t msg;
//Create dashboard_can object
DashboardCAN dashboard_can(&stm_can);

DMA_HandleTypeDef hdma_spi1_tx;
void initDMA(void) {

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

  hdma_spi1_tx.Instance = DMA2_Stream3;
  hdma_spi1_tx.Init.Channel = DMA_CHANNEL_3;
  hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  hdma_spi1_tx.Init.Mode = DMA_NORMAL;
  hdma_spi1_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
  hdma_spi1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK) {
    Error_Handler();
  }

  SPI_HandleTypeDef* SPI_handle = SPI.getHandle();

  // Link SPI and DMA?
  SPI_handle->hdmatx = &hdma_spi1_tx;
  hdma_spi1_tx.Parent = SPI_handle;

  HAL_SPI_Transmit_DMA

}


void setup(void)
{

  // set non-needed Display pins low
  pinMode(PC5, OUTPUT);
  pinMode(PB1, OUTPUT);
  pinMode(PA3, OUTPUT);
  digitalWrite(PC5, LOW);
  digitalWrite(PB1, LOW);
  
  // set LED high
  digitalWrite(PA3, HIGH);

  // begin usb serial for STM32
  SerialUSB.begin();

  SerialUSB.println("HELLO");

  // run startup sequence for dasboard
  dashboard->startup();

  // initialize buttons and other controls
  dash_controls.startup();
}

void loop(void) {
  int time = millis();
  // read can messages from CAN bus
  dashboard_can.read_CAN();

  // update controls (buttons, knobs, etc.)
  dash_controls.update(&dashboard_can);

  // send dashboard status message
  dashboard_can.send_status();
  
  // refresh dashboard (display and neopixels)
  dashboard->refresh((DashboardCAN*) &dashboard_can);
  SerialUSB.println(millis() - time);
}


