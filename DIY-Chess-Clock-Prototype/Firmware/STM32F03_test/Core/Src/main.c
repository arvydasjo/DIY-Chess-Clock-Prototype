/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {CONFIG, READYTOPLAY, PLAY, FINISH} gameStatus_TypeDef;
typedef enum {LEFT, RIGHT} gamePlayer_TypeDef;
typedef enum {ColLeftMin = 3, ColLeftSec = 6, ColRightMin = 10, ColRightSec = 13} CursorCol_TypeDef;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBOUNCE_MS 10 // ms

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim6;

/* USER CODE BEGIN PV */
static uint8_t LCD_ADR = 0x3E << 1;

gameStatus_TypeDef gameStatus = CONFIG;
gamePlayer_TypeDef gamePlayer = LEFT;
CursorCol_TypeDef cursorCol = ColLeftMin;

 int minLeft = 5;
 int secLeft = 0;

 int minRight = 5;
 int secRight = 0;

 int timeLeft;
 int timeRight;

 uint8_t textLCD[] =            "  00:00  00:00  ";
 uint8_t textLCDConfig[] = 		"  Adjust time   ";
 uint8_t textLCDReadytoPlay[] = " Ready to play  ";
 uint8_t textLCDPlaying[] = 	"    Playing     ";

 uint8_t textLCDWinL[] =        "  Won    Lost   ";
 uint8_t textLCDWinR[] =        "  Lost   Won    ";

char temp[5];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
void LCDInit(void);
void LCDInitConfig(void);

void LCDClear(void);
void LCDSetCursor(uint8_t col, uint8_t row);
void LCDPrintStr(uint8_t *str, uint8_t len);
void LCDBlinkOn(void);
void LCDBlinkOff(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  timeLeft = minLeft*60 + secLeft;
  timeRight = minRight*60 + secRight;


  HAL_TIM_Base_Start_IT(&htim6);

  LCDInit();
  LCDSetCursor(cursorCol, 0);

  while (1)
  {
	//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	//HAL_Delay(1000);

	if(gameStatus == CONFIG){
		timeLeft = minLeft*60 + secLeft;
		timeRight = minRight*60 + secRight;
	}

	sprintf(textLCD, "  %02d:%02d  %02d:%02d  ", timeLeft / 60, timeLeft % 60, timeRight / 60, timeRight % 60);
	//memcpy(&textLCD[2], temp, 5);

	//sprintf(temp, "%02d:%02d", timeRight / 60, timeRight % 60);
	//memcpy(&textLCD[9], temp, 5);

	  LCDClear();
	  LCDInit();

	  LCDSetCursor(0, 0);
	  LCDPrintStr(textLCD, 16);


	  if(gameStatus == READYTOPLAY){
		  LCDSetCursor(0, 1);
		  LCDPrintStr(textLCDReadytoPlay, 16);
	  }

	  if(gameStatus == FINISH && timeLeft == 0 && timeRight != 0){
		  LCDSetCursor(0, 1);
		  LCDPrintStr(textLCDWinR, 16);
	  }

	  if(gameStatus == FINISH && timeRight == 0 && timeLeft != 0){
		  LCDSetCursor(0, 1);
		  LCDPrintStr(textLCDWinL, 16);
	  }

	  if(gameStatus == CONFIG){
		  LCDSetCursor(0, 1);
		  LCDPrintStr(textLCDConfig, 16);
	  }

	  if(gameStatus == PLAY){
		  LCDSetCursor(0, 1);
		  LCDPrintStr(textLCDPlaying, 16);
	  }

	  if(gameStatus == CONFIG) LCDBlinkOn();
	  	  else LCDBlinkOff();

	  LCDSetCursor(cursorCol, 0);

	  HAL_Delay(500);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00201D2B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 7999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA3 PA4 PA5 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void LCDInit(){
	uint8_t arr[2];
	arr[0]=0;

	arr[1]=0xC;
	HAL_I2C_Master_Transmit(&hi2c1,LCD_ADR,arr,2,1000); //Sending in Blocking mode

	arr[1]=0x3F;
	HAL_I2C_Master_Transmit(&hi2c1,LCD_ADR,arr,2,1000); //Sending in Blocking mode

}

void LCDInitConfig(){
	uint8_t arr[2];
	arr[0]=0;

	arr[1]=0xF;
	HAL_I2C_Master_Transmit(&hi2c1,LCD_ADR,arr,2,1000); //Sending in Blocking mode

	arr[1]=0x3F;
	HAL_I2C_Master_Transmit(&hi2c1,LCD_ADR,arr,2,1000); //Sending in Blocking mode

}

void LCDBlinkOn(void) {
    uint8_t arr[2];
    arr[0] = 0x00;    // control byte: command
    arr[1] = 0x0E;    // Display ON, Cursor ON, Blink ON
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADR, arr, 2, 1000);
}

void LCDBlinkOff(void) {
    uint8_t arr[2];
    arr[0] = 0x00;    // command
    arr[1] = 0x0C;    // Display ON, Cursor OFF, Blink OFF
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADR, arr, 2, 1000);
}

void LCDClear(void){
	  uint8_t arr[2];
		arr[0]=0x0;
		arr[1]=0x1;
		HAL_I2C_Master_Transmit(&hi2c1,LCD_ADR,arr,2,1000); //Sending in Blocking mode


}

void LCDSetCursor(uint8_t col, uint8_t row) {

    uint8_t arr[2];
    col = (row == 0 ? col | 0x80 : col | 0xc0);
		arr[0]=0x80;
		arr[1]=col;
		HAL_I2C_Master_Transmit(&hi2c1,LCD_ADR,arr,2,1000); //Sending in Blocking mode
}


void LCDPrintStr(uint8_t *str, uint8_t len){

	uint8_t arr[2];
	uint8_t i;
	arr[0]=0x40;

	for(i=0;i<len;i++){
		arr[1]=str[i];
		HAL_I2C_Master_Transmit(&hi2c1,LCD_ADR,arr,2,1000); //Sending in Blocking mode
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if(htim->Instance == TIM6){


		uint16_t suma = 0;

		for(uint16_t i = 0;i < 1000;i++)
		{
			suma+=(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)==GPIO_PIN_SET) ? 0 : 1;
		}

		if(suma > 900)
			gamePlayer = RIGHT;
		else
			gamePlayer = LEFT;


		if(gameStatus == PLAY){
			if(gamePlayer == LEFT)timeLeft --;
				else(timeRight --);

			if(timeLeft == 0 || timeRight == 0) gameStatus = FINISH;

		}


	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	static uint32_t last_tick7 = 0;
	static uint32_t last_tick6 = 0;
	static uint32_t last_tick5 = 0;
	static uint32_t last_tick4 = 0;
	static uint32_t last_tick3 = 0;


    if (GPIO_Pin == GPIO_PIN_6 && gameStatus == CONFIG) {
    	if (HAL_GetTick() - last_tick6 > DEBOUNCE_MS) {

    		last_tick6 = HAL_GetTick();

    		if(cursorCol == ColLeftMin) minLeft++;
    		if (minLeft == 61) minLeft = 0;

    		if(cursorCol == ColLeftSec) secLeft++;
    		if(secLeft == 60) secLeft = 0;

    		if(cursorCol == ColRightMin) minRight++;
    		if(minRight == 61) minRight = 0;

    		if(cursorCol == ColRightSec) secRight++;
    		if(secRight == 60) secRight = 0;

    		/*HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);*/

    	 }

       }

    if (GPIO_Pin == GPIO_PIN_5 && gameStatus == CONFIG) {
       	if (HAL_GetTick() - last_tick5 > DEBOUNCE_MS) {

       		last_tick5 = HAL_GetTick();

       		if(cursorCol == ColLeftMin) minLeft--;
       		if (minLeft == -1) minLeft = 60;

       		if(cursorCol == ColLeftSec) secLeft--;
       		if(secLeft == -1) secLeft = 59;

       		if(cursorCol == ColRightMin) minRight--;
       		if(minRight == -1) minRight = 60;

       		if(cursorCol == ColRightSec) secRight--;
       		if(secRight == -1) secRight = 59;

       	 }

      }

    if (GPIO_Pin == GPIO_PIN_4 && gameStatus == CONFIG) {
       	if (HAL_GetTick() - last_tick4 > DEBOUNCE_MS) {

       		last_tick4 = HAL_GetTick();

       		int c = cursorCol;

       		switch(c) {
       		    case ColLeftMin:  cursorCol = ColLeftSec;   break;
       		    case ColLeftSec:  cursorCol = ColRightMin;  break;
       		    case ColRightMin: cursorCol = ColRightSec;  break;
       		    case ColRightSec: cursorCol = ColLeftMin;
       		}

       	}
    }

    if (GPIO_Pin == GPIO_PIN_3) {
       	if (HAL_GetTick() - last_tick3 > DEBOUNCE_MS) {

       		last_tick3 = HAL_GetTick();

       		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

       		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_RESET)
       		{
       			if(gameStatus == CONFIG) gameStatus = READYTOPLAY;
       			if(gameStatus == PLAY || gameStatus == FINISH) gameStatus = CONFIG;
       		}
       	}
    }




    if (GPIO_Pin == GPIO_PIN_7) {
       	if (HAL_GetTick() - last_tick7 > DEBOUNCE_MS) {

       		last_tick7 = HAL_GetTick();

       		if(gameStatus == READYTOPLAY) gameStatus = PLAY;




       	}

    }

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
