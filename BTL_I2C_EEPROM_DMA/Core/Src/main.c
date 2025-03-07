/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stdint.h"
#include "EEPROM.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
uint8_t str1_readbyte[50] = "0A550402";
uint8_t str3_readbyte[50] = "0A550B03";
uint8_t str2_readbyte[50] = "0D0A";

uint8_t receive_data[47];
uint8_t buffer[100] = {0};
int n_cnt = 0;
uint8_t dmaDataReceived = 0;
int expected_size;
int data_size;
uint8_t size = 0;
int cal_size;
char result[32];
char result_b[18];
#define EEPROM_I2C_write &hi2c1
#define EEPROM_ADDR_write 0xA0

char str1_w[2];
char str2_w[2];
char str1_r[2];
char str2_r[2];
char str3[2];
char str1_padd[2];
char str2_padd[2];
char str1_badd[2];
char str2_badd[2];
char combinedstr_add[4];
char combinedstr_badd[4];
char combinedstr[5];
uint8_t datar1[100];
uint8_t datar2[16];
uint8_t datar3[2];

uint16_t byte_page1;
uint16_t byte_page2;
uint16_t pagesum;

uint16_t byte_write1;
uint16_t byte_write2;
uint16_t bytesum;

uint16_t byte_read1;
uint16_t byte_read2;
uint16_t byte_read3;
uint16_t byte_read4;
unsigned short xxxx;

int firstNumber;
int secondNumber;
uint16_t bytesum_read;
uint16_t byte_write_data;

char hexString[16];
char hexString_byte[16];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//if (huart->Instance == USART2)
//	{
//	HAL_UART_Receive_DMA(&huart2,&buffer[0],sizeof(buffer));
//	n_cnt++;
//	strcpy(receive_data, buffer);
//	dmaDataReceived = 1;
//	}
//
//}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, buffer, 100);
    size = Size;
    if (size == sizeof(receive_data))
    {
    	strcpy(receive_data, buffer);
    	dmaDataReceived = 1;
    	n_cnt++;
    }
}


void removeSpaces(char *str) {
    int len = strlen(str);
    int i, j = 0;

    for (i = 0; i < len; i++) {
        if (!isspace((unsigned char)str[i])) {
            str[j++] = str[i];
        }
    }

    str[j] = '\0';  // Thêm ký tự null để kết thúc chuỗi
}
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  //HAL_UART_Receive_DMA(&huart2,&buffer[0],sizeof(buffer));
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, buffer, 100);
  __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  if (dmaDataReceived==1)
	  {

	  sscanf(&receive_data[6], "%2x", &expected_size);
	  char *end_ptr;
	  long int address = strtol(expected_size, &end_ptr, 16);
	  if (strncmp((const char*)receive_data, "0A 55", 5) == 0)				//START BYTE
	  {
		  data_size= strlen(receive_data);
		  cal_size = (data_size - 1 - 2 - 12 - (expected_size - 1))/2;
		  if (cal_size == expected_size)
		  {

		  if (strncmp((const char*)receive_data + 9, "01", 2) == 0)
			{
				  sscanf(&receive_data[12], "%2x", &byte_page1);			//01
				  sscanf(&receive_data[15], "%2x", &byte_page2);			//17h --> 23d
				  pagesum = (uint16_t)((byte_page1 << 8) | byte_page2);		//279
				  char *start_position = &receive_data[18];
				  EEPROM_PageErase(pagesum);


				  char *end_position = strstr(start_position, "0D 0A");

				  if (end_position != NULL)
				  {

				      size_t length = end_position - start_position;
				      hexString[length];
				      strncpy(hexString, start_position, length);
				      removeSpaces(hexString);
				      EEPROM_Write(pagesum, 0, hexString, strlen((char *)hexString));

				  }

			  }
		  else if (strncmp((const char*)receive_data + 9, "00", 2) == 0)
			  {

				  sscanf(&receive_data[12], "%s", &str1_w);
				  sscanf(&receive_data[15], "%s", &str2_w);
				  strcpy(combinedstr, str1_w);
				  strcat(combinedstr, str2_w);
				  xxxx = (unsigned short)strtol(combinedstr, NULL, 16);
				  firstNumber = (xxxx) & 0x3F;
				  secondNumber = (xxxx & 0x7FC0)>>6;
				  sscanf(&receive_data[18], "%s", &str3);
				  strcpy(hexString_byte, str3);
				  EEPROM_Write(secondNumber, firstNumber, hexString_byte, strlen((char *)hexString_byte));
			  }

		  else if (strncmp((const char*)receive_data + 9, "02", 2) == 0)		// current read
			  {

				  sscanf(&receive_data[12], "%s", &str1_r);
				  sscanf(&receive_data[15], "%s", &str2_r);
				  strcpy(combinedstr, str1_r);
				  strcat(combinedstr, str2_r);
				  xxxx = (unsigned short)strtol(combinedstr, NULL, 16);

				  firstNumber = (xxxx) & 0x3F;
				  secondNumber = (xxxx & 0x7FC0)>>6;
				  EEPROM_Read(secondNumber, firstNumber, datar3, 2);
				  sscanf(&receive_data[12], "%s", &str1_badd);
			      sscanf(&receive_data[15], "%s", &str2_badd);

				  sprintf(combinedstr_badd, "%s%s", str1_badd, str2_badd);
				  int index = 0;
				  for (int i = 0; i < strlen(str1_readbyte); i++)
				  {
					  result_b[index++] = str1_readbyte[i];
				  }
				  for (int i = 0; i < strlen(combinedstr_badd); i++) {
					  result_b[index++] = combinedstr_badd[i];
				  }
				  for (int i = 0; i < sizeof(datar3); i++)
				  {
					  result_b[index++] = datar3[i];
					  if (datar3[i] == 255)
					  {
						  datar3[i] = '0';
					  }
					  HAL_Delay(10);
				  }
				  for (int i = 0; i < strlen(str2_readbyte); i++) {
					  result_b[index++] = str2_readbyte[i];
				  }
				  HAL_UART_Transmit_DMA(&huart2, result_b, sizeof(result_b));
				  HAL_Delay(10);
				  dmaDataReceived=0;



			  }

			 else if (strncmp((const char*)receive_data + 9, "03", 2) == 0)		// sequential read
			  {
			 sscanf(&receive_data[12], "%2x", &byte_page1);			//01
				  sscanf(&receive_data[15], "%2x", &byte_page2);			//17h --> 23d
				  pagesum = (uint16_t)((byte_page1 << 8) | byte_page2);
				  EEPROM_Read(pagesum, 0, datar2, 16);
				  sscanf(&receive_data[12], "%s", &str1_padd);
				  sscanf(&receive_data[15], "%s", &str2_padd);

				  sprintf(combinedstr_add, "%s%s", str1_padd, str2_padd);


				    int index = 0;
				        for (int i = 0; i < strlen(str3_readbyte); i++) {
				            result[index++] = str3_readbyte[i];
				        }
				        for (int i = 0; i < strlen(combinedstr_add); i++) {
				            result[index++] = combinedstr_add[i];
				        }
				        for (int i = 0; i < sizeof(datar2); i++) {
				        	if (datar2[i] == 255)
				        	{
				        		datar2[i] = '0';

				        	}
				            result[index++] = datar2[i];

				            HAL_Delay(10);
				        }
				        for (int i = 0; i < strlen(str2_readbyte); i++) {
				            result[index++] = str2_readbyte[i];
				        }
				        HAL_UART_Transmit_DMA(&huart2, result, sizeof(result));
				        HAL_Delay(10);
				        dmaDataReceived=0;
				    }
	  }
			  }

			  dmaDataReceived=0;
	  }


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
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
