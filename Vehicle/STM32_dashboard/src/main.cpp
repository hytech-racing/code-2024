#include "main.h"
// defined pins for SPI display
#define SHARP_SCK  PA5
#define SHARP_MOSI PA7
#define SHARP_SS   PC4

//defined black and white values for display
#define BLACK 0
#define WHITE 1
//^^ both of those will prob. be moved to hytech_dashboard

// STM32 clock config for 24Mhz xtal, generated in CUBE IDE
// extern "C" overrides configuration written in board variant
extern "C" void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLRCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

// This might be needed for USB Serial
extern PCD_HandleTypeDef g_hpcd;
extern "C" void USB_LP_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&g_hpcd);
}

//Create STM32_CAN object to pass to DashboardCAN
STM32_CAN stm_can( CAN2, DEF);
DashboardCAN dashboard_can(&stm_can);

void setup(void)
{

  //set non-needed Display pins low
  //turn on LED
  pinMode(PC5, OUTPUT);
  pinMode(PB1, OUTPUT);
  pinMode(PA3, OUTPUT);
  digitalWrite(PC5, LOW);
  digitalWrite(PB1, LOW);
  //set LED high
  digitalWrite(PA3, HIGH);

  SerialUSB.begin();

  hytech_dashboard::getInstance().startup();
}

void loop(void) 
{
  dashboard_can.read_CAN();
  hytech_dashboard::getInstance().refresh(&dashboard_can);
}


