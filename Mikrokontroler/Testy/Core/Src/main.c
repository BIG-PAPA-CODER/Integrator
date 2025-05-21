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
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"
#include "vl53l5cx_api.h"
#include "AMG8833.h"
#include "stm32l4xx_hal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RESIZE_X 128
#define RESIZE_Y 128
#define TA_SHIFT 8

#define CRC16 0x1021
#define CRC16_INIT 0
#define CRC16_POLYNOMIAL 0x8005
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float mlx90640To[768];
uint16_t eeMLX90640[832];
paramsMLX90640 mlx90640;
uint16_t mlx90640Frame[834];
int status3=0;

int status, status2;
int startVL1 = 0;
int startVL2 = 0;
uint8_t i, isReady, isReady2;
VL53L5CX_Configuration 	Dev, Dev2;
VL53L5CX_ResultsData 	Results, Results2;
uint8_t resolution, resolution2, isAlive, isAlive2;

float pixels[64];
int16_t pixelsRaw[64];
int16_t pixelsRawResize[RESIZE_X*RESIZE_Y];
uint16_t buf[64];

char flag = 'B';
uint8_t Rx_data;

uint16_t crc_result;

uint16_t test = 12345;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint16_t calculateCRC16(uint16_t *data, int length);
void get_data_by_polling(VL53L5CX_Configuration *p_dev);
void get_data_by_interrupt(VL53L5CX_Configuration *p_dev);
void get_result_VL53L5CX1();
void get_result_VL53L5CX2();
void get_result_MLX90640();
void get_result_AMG8833();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Funkcja obliczająca CRC-16
//uint16_t calculateCRC16(uint16_t *data, int length) {
//    /*uint16_t crc = 0xFFFF;
//    for (int i = 0; i < length; i++) {
//        crc ^= data[i] << 8;
//        for (int j = 0; j < 8; j++) {
//            if (crc & 0x8000) {
//                crc = (crc << 1) ^ CRC16;
//            } else {
//                crc = (crc << 1);
//            }
//        }
//    }
//    return crc;*/
//
//    uint16_t out = 0xFFFF;
//    int bits_read = 0, bit_flag;
//
//
//    if(data == NULL)
//        return 0;
//
//    while(length > 0)
//    {
//        bit_flag = out >> 15;
//
//        out <<= 1;
//        out |= (*data >> bits_read) & 1; // item a) work from the least significant bits
//
//        bits_read++;
//        if(bits_read > 7)
//        {
//            bits_read = 0;
//            data++;
//            length--;
//        }
//
//
//        if(bit_flag)
//            out ^= CRC16;
//
//    }
//
//    uint16_t i;
//    for (i = 0; i < 16; ++i) {
//        bit_flag = out >> 15;
//        out <<= 1;
//        if(bit_flag)
//            out ^= CRC16;
//    }
//
//    uint16_t crc = 0;
//    i = 0x8000;
//    int j = 0x0001;
//    for (; i != 0; i >>=1, j <<= 1) {
//        if (i & out) crc |= j;
//    }
//
//    return crc;
//}

unsigned short int ComputeCRC16(const char* pData, int Length, unsigned int Poly, unsigned short int InitVal) {
    unsigned short int ResCRC = InitVal;

    while (--Length >= 0) {
        ResCRC ^= *pData++ << 8;
        for (short int i = 0; i < 8; ++i) {
            ResCRC = ResCRC & 0x8000 ? (ResCRC << 1) ^ Poly : ResCRC << 1;
        }
    }
    return ResCRC & 0xFFFF;
}


int _write(int file, char *ptr, int len){
	HAL_UART_Transmit(&huart2, ptr, len, 100);
	//HAL_UART_Transmit_IT(&huart2, ptr, len);
	return len;
}

/*int _read(int file, char *ptr, int len){
	HAL_UART_Receive(&huart2, ptr, 1, HAL_MAX_DELAY);
	return len;
}*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	switch (Rx_data) {

	case 'A':
		flag = 'A';
		break;
	case 'B':
		flag = 'B';
		break;
	case 'C':
		flag = 'C';
		break;
	case 'D':
		flag = 'D';
		break;
	case 'E':
		flag = 'E';
		break;
	case 'F':
		flag = 'F';
		break;
	case 'G':
		flag = 'G';
		break;
		/*ITS A NEW SECTON*/
	case 'H':
			flag = 'H';
			break;
	case 'I':
			flag = 'I';
			break;
	default:
		printf("Nieobslugiwany przypadek \n");
		break;
	}
	printf("Flaga: %c\n", flag);
	HAL_UART_Transmit_IT(&huart2, &Rx_data, 1);
	HAL_UART_Receive_IT(&huart2, &Rx_data, 1);
}


void get_result_VL53L5CX1(){
	status = vl53l5cx_check_data_ready(&Dev, &isReady);
	if(isReady)
	{
		vl53l5cx_get_resolution(&Dev, &resolution);
		vl53l5cx_get_ranging_data(&Dev, &Results);

		//crc_result = calculateCRC16((uint16_t*)&Results.distance_mm, sizeof(Results.distance_mm));
		crc_result = ComputeCRC16((char*)&Results.distance_mm, sizeof(Results.distance_mm), CRC16_POLYNOMIAL, CRC16_INIT);

		//printf("\r\n==========================DANE Z PIERWSZEGO CZUJNIKA VL53LC5X==========================\r\n");
		//printf("\r\n============================================================================\r\n");
		printf("X %d ",sizeof(Results.distance_mm)+6);
		for(i = 0; i < resolution; i++)
		{
			/*if(i%4 == 0 && i != 0){
				printf("\r\n");
			}*/
		  	printf("%d ", Results.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
		}
		printf("%04X Y\r\n", crc_result);
	}
	WaitMs(&(Dev.platform), 5);

}

void get_result_VL53L5CX2(){
	status2 = vl53l5cx_check_data_ready2(&Dev2, &isReady2);
	if(isReady2)
	{
		vl53l5cx_get_resolution2(&Dev2, &resolution2);
		vl53l5cx_get_ranging_data2(&Dev2, &Results2);
 //	crc_result = calculateCRC16((uint16_t*)&Results2.distance_mm, sizeof(Results2.distance_mm));
		crc_result = ComputeCRC16((char*)&Results2.distance_mm, sizeof(Results2.distance_mm), CRC16_POLYNOMIAL, CRC16_INIT);

		//printf("\r\n==========================DANE Z DRUGIEGO CZUJNIKA VL53LC5X==========================\r\n");
		//printf("\r\n============================================================================\r\n");
		printf("Z %d ",sizeof(Results2.distance_mm)+6);
		for(i = 0; i < resolution2; i++)
		{
			/*if(i%4 == 0 && i != 0){
				printf("\r\n");
			}*/
		  	printf("%d ", Results2.distance_mm[VL53L5CX_NB_TARGET_PER_ZONE*i]);
		}
		printf("%04X Y\r\n", crc_result);
	}
	WaitMs(&(Dev2.platform), 5);

}

void get_result_MLX90640(){
	status3 = MLX90640_GetFrameData(mlx90640Frame);

	float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
	float tr = Ta - TA_SHIFT;
	float emissivity = 0.95;
	MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
	//crc_result = calculateCRC16((uint16_t*)&mlx90640To, sizeof(mlx90640To));
	crc_result = ComputeCRC16((char*)&mlx90640To, sizeof(mlx90640To), CRC16_POLYNOMIAL, CRC16_INIT);
	//printf("\r\n==========================DANE Z CZUJNIKA MLX90640==========================\r\n");
	printf("L %d ",sizeof(mlx90640To)+6);
	for(int i = 0; i < 768; i++){
		/*if(i%32 == 0 && i != 0){
			printf("\r\n");
		}*/
		printf("%2.2f ",mlx90640To[i]);
	}
	printf("%04X Y\r\n", crc_result);
}

void get_result_AMG8833(){
	//printf("\r\n============================================================================\r\n");
	//printf("\r\n==========================DANE Z CZUJNIKA AMG8833===========================\r\n");
	readPixels(pixels, 64);
	//crc_result = calculateCRC16((uint16_t*)&pixels, sizeof(pixels));
	crc_result = ComputeCRC16((char*)&pixels, sizeof(pixels), CRC16_POLYNOMIAL, CRC16_INIT);
	printf("P %d ",sizeof(pixels)+6);
	for(int i = 0; i < 64; i++){
		/*if(i%8 == 0 && i != 0){
			printf("\r\n");
		}*/
		printf("%2.2f ",pixels[i]);
	}
	printf("%04X Y\r\n", crc_result);
}

void show_menu(){
	printf("A - Zbieranie danych ze wszystkich czujników\n");
	printf("B - Zbieranie danych z pierwszego czujnika VL53L5CX\n");
	printf("C - Zbieranie danych z drugiego czujnika VL53L5CX\n");
	printf("D - Zbieranie danych z czujnika MLX90640\n");
	printf("E - Zbieranie danych z czujnika AMG8833\n");
	printf("F - Zbieranie danych z czujnika AMG8833 oraz czujnikow odleglosci\n");
	printf("G - Zbieranie danych z czujnika MLX90640 oraz czujnikow odleglosci\n");
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
  printf("%04X \r\n", calculateCRC16(test, sizeof(test)));
  HAL_UART_Receive_IT(&huart2, &Rx_data, 1);
  printf("Inicjalizacja czujnika AMG8833...\n");
  amg88xxInit();
  printf("Koniec inicjalizacji\n");

  printf("Inicjalizacja czujnika MLX90640...\n");
  Init_MLX90640_GPIO(hi2c1);
  MLX90640_SetRefreshRate(MLX90640_RATE_1HZ);
  MLX90640_SetResolution(MLX90640_RES16);
  MLX90640_SetPattern(MLX90640_CHESS);
  MLX90640_SetMode(MLX90640_DEFAULT);
  MLX90640_DumpEE(eeMLX90640);
  status3 = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
  printf("Koniec inicjalizacji\n");

  HAL_Delay(10);

  Dev.platform.address = VL53L5CX_DEFAULT_I2C_ADDRESS;
  status = vl53l5cx_is_alive(&Dev, &isAlive);
  printf("Inicjalizacja pierwszego czujnika VL53L5CX...\n");
  status = vl53l5cx_init(&Dev);
  status = vl53l5cx_set_resolution(&Dev, VL53L5CX_RESOLUTION_8X8);
  status = vl53l5cx_set_ranging_frequency_hz(&Dev, 1);
  status = vl53l5cx_set_target_order(&Dev, VL53L5CX_TARGET_ORDER_CLOSEST);
  status = vl53l5cx_set_ranging_mode(&Dev, VL53L5CX_RANGING_MODE_CONTINUOUS);
  printf("Koniec inicjalizacji\n");

  Dev2.platform.address = VL53L5CX_DEFAULT_I2C_ADDRESS;
  status2 = vl53l5cx_is_alive2(&Dev2, &isAlive2);
  printf("Inicjalizacja drugiego czujnika VL53L5CX...\n");
  status2 = vl53l5cx_init2(&Dev2);
  status2 = vl53l5cx_set_resolution2(&Dev2, VL53L5CX_RESOLUTION_8X8);
  status2 = vl53l5cx_set_ranging_frequency_hz2(&Dev2, 1);
  status2 = vl53l5cx_set_target_order2(&Dev2, VL53L5CX_TARGET_ORDER_CLOSEST);
  status2 = vl53l5cx_set_ranging_mode2(&Dev2, VL53L5CX_RANGING_MODE_CONTINUOUS);
  printf("Koniec inicjalizacji\n");

  show_menu();
  printf("Wybierz opcje: \n");
  //flag = getchar();
  printf("Wybrana opcja: %c\n", flag);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  switch(flag){
	  case 'A':
		  if (startVL1 == 0 || startVL2 == 0){
			  status = vl53l5cx_start_ranging(&Dev);
			  startVL1 = 1;
			  status2 = vl53l5cx_start_ranging2(&Dev2);
			  startVL2 = 1;
		  }
		  get_result_VL53L5CX1();
		  get_result_VL53L5CX2();
		  get_result_MLX90640();
		  get_result_AMG8833();
		  HAL_Delay(1000);
		  break;
	  case 'B':
		  if (startVL1 == 0){
			  status = vl53l5cx_start_ranging(&Dev);
			  startVL1 = 1;
			  status2 = vl53l5cx_stop_ranging2(&Dev2);
			  startVL2 = 0;
		  }
		  get_result_VL53L5CX1();
		  HAL_Delay(1000);
		  break;
	  case 'C':
		  if (startVL2 == 0){
			  status2 = vl53l5cx_start_ranging2(&Dev2);
			  startVL2 = 1;
			  status = vl53l5cx_stop_ranging(&Dev);
			  startVL1 = 0;
		  }
		  get_result_VL53L5CX2();
		  HAL_Delay(1000);
		  break;
	  case 'D':
		  if (startVL1 == 1 || startVL2 == 1){
			  status = vl53l5cx_stop_ranging(&Dev);
			  startVL1 = 0;
			  status2 = vl53l5cx_stop_ranging2(&Dev2);
			  startVL2 = 0;
		  }
		  get_result_MLX90640();
		  HAL_Delay(1000);
		  break;
	  case 'E':
		  if (startVL1 == 1 || startVL2 == 1){
			  status = vl53l5cx_stop_ranging(&Dev);
			  startVL1 = 0;
			  status2 = vl53l5cx_stop_ranging2(&Dev2);
			  startVL2 = 0;
		  }
		  get_result_AMG8833();
		  HAL_Delay(1000);
		  break;
	  case 'F':
		  if (startVL1 == 0 || startVL2 == 0){
			  status = vl53l5cx_start_ranging(&Dev);
			  startVL1 = 1;
			  status2 = vl53l5cx_start_ranging2(&Dev2);
			  startVL2 = 1;
		  }
		  get_result_VL53L5CX1();
		  get_result_VL53L5CX2();
		  get_result_AMG8833();
		  HAL_Delay(1000);
		  break;
	  case 'G':
		  if (startVL1 == 0 || startVL2 == 0){
			  status = vl53l5cx_start_ranging(&Dev);
			  startVL1 = 1;
			  status2 = vl53l5cx_start_ranging2(&Dev2);
			  startVL2 = 1;
		  }
		  get_result_VL53L5CX1();
		  get_result_VL53L5CX2();
		  get_result_MLX90640();
		  HAL_Delay(1000);
		  break;
		  /*ITS A NEW SECTON*/
	  case 'H':
		  if (startVL1 == 0 || startVL2 == 0){
			  status = vl53l5cx_start_ranging(&Dev);
			  startVL1 = 1;
			  status2 = vl53l5cx_start_ranging2(&Dev2);
			  startVL2 = 1;
		  }
		  get_result_VL53L5CX1();
		  get_result_VL53L5CX2();
		  HAL_Delay(1000);
		  break;
	  case 'I':
		  if (startVL1 == 1 || startVL2 == 1){
			  status = vl53l5cx_stop_ranging(&Dev);
			  startVL1 = 0;
			  status2 = vl53l5cx_stop_ranging2(&Dev2);
			  startVL2 = 0;
		  }
		  get_result_MLX90640();
		  get_result_AMG8833();
		  HAL_Delay(1000);

		  break;

	  default:
		  break;
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
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
