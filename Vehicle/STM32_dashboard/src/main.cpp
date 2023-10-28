#include "main.h"

// STM32 clock config for 24Mhz xtal, generated in CUBE IDE
// extern "C" overrides configuration written in board variant
extern "C" void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

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

  //set peripheral clocks for USB functionality (this is not STM32CUBEIDE code)
  //This was found in the stm32duino variants clock config, and was modified
  //to match the clock config in CUBEIDE
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDIO | RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLSAI.PLLSAIM = 12;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 96;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP;
  PeriphClkInitStruct.SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    Error_Handler();
  }
}

// This might be needed for USB Serial
extern PCD_HandleTypeDef g_hpcd;
extern "C" void USB_LP_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&g_hpcd);
}

// initialize/construct singleton object(this was a nightmare to setup and
// can probably be replaced at some point with traditional class)
hytech_dashboard* hytech_dashboard::_instance = NULL;
// get instance and set to dashboard pointer for easier reference
hytech_dashboard* dashboard = hytech_dashboard::getInstance();

//Create STM32_CAN object to pass to DashboardCAN
STM32_CAN stm_can( CAN2, DEF);
CAN_message_t msg;
//Create dashboard_can object
DashboardCAN dashboard_can(&stm_can);

void setup(void)
{

  //set non-needed Display pins low
  pinMode(PC5, OUTPUT);
  pinMode(PB1, OUTPUT);
  pinMode(PA3, OUTPUT);
  digitalWrite(PC5, LOW);
  digitalWrite(PB1, LOW);
  //set LED high
  digitalWrite(PA3, HIGH);

  //begin usb serial for STM32
  SerialUSB.begin();

  SerialUSB.println("HELLO");

  //run startup sequence for dasboard
  dashboard->startup();
}

void loop(void) 
{
  //read can messages from CAN bus
  dashboard_can.read_CAN();
  //send dashboard status message
  dashboard_can.send_status();
  
  //refresh dashboard (display and neopixels)
  dashboard->refresh((DashboardCAN*) &dashboard_can);
}


