/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "task.h"
#include <stdio.h>
#include "queue.h"
#include "limits.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//启动任务配置参数
#define START_PRIO 1
#define START_STACK_SIZE 128
TaskHandle_t StartTask_Handler;
void Start_Task(void *pvParameters);

//接收任务配置参数
#define REC1_PRIO 3
#define REC1_STACK_SIZE 128
TaskHandle_t rec1_Handler;
void Rec1_Task(void *pvParameters);

//接收任务配置参数
#define REC2_PRIO 2
#define REC2_STACK_SIZE 128
TaskHandle_t rec2_Handler;
void Rec2_Task(void *pvParameters);

//发送任务配置参数
#define SEND_PRIO 4
#define SEND_STACK_SIZE 128
TaskHandle_t send_Handler;
void Send_Task(void *pvParameters);

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

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
  MX_USART1_UART_Init();
	
  /* USER CODE BEGIN 2 */
	//通过串口向pc发送数据
	printf("usart init success\n");
	//创建启动任务
   xTaskCreate(Start_Task,"Start_Task",START_STACK_SIZE,NULL,START_PRIO,&StartTask_Handler);
   //启动调度器
	 vTaskStartScheduler();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Start_Task(void *pvParameters){ 
  taskENTER_CRITICAL();
  xTaskCreate(Rec2_Task,"Rec2_Task",REC2_STACK_SIZE,NULL,REC2_PRIO,&rec2_Handler);
  xTaskCreate(Rec1_Task,"Rec1_Task",REC1_STACK_SIZE,NULL,REC1_PRIO,&rec1_Handler);
  xTaskCreate(Send_Task,"Send_Task",SEND_STACK_SIZE,NULL,SEND_PRIO,&send_Handler);

  vTaskDelete(NULL); 
  taskEXIT_CRITICAL();
}


void Rec1_Task(void *pvParameters){
  BaseType_t res=pdFALSE;
  while(1){
    printf("rec1_task ready for running,wait semphr\n");
   res= ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
   if(res==pdTRUE){
    printf("rec1_task semphr receive success,rec1_task running\n");
   }else{
      printf("rec1_task semphr receive fail,rec1_task begin fail");
   }
    osDelay(20);
  }
}  

void Rec2_Task(void *pvParameters){
  BaseType_t res=pdFALSE;
  while(1){
    printf("rec2_task ready for running,wait semphr\n");
   res= ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
   if(res==pdTRUE){
    printf("rec2_task semphr receive success,rec2_task running\n");
   }else{
      printf("rec2_task semphr receive fail,rec2_task begin fail");
   }
    osDelay(20);
  }
}  


void Send_Task(void *pvParameters){
  uint8_t keyNum = 0;
  BaseType_t res=pdFALSE;

  while(1){
   keyNum=Key_GetKeyNum();
   if(keyNum==1){
    printf("%s\n","key1 click,send semphr to task1\n");
    res=xTaskNotifyGive(rec1_Handler);
      if(res==pdTRUE){
        printf("semphr send to task1 success\n");
      }else{
        printf("semphr send to task1 fail\n");
      }
   }else if(keyNum==2){
     printf("%s\n","key2 click,send semphr to task2\n");
    res=xTaskNotifyGive(rec2_Handler);
      if(res==pdTRUE){
        printf("semphr send to task2 success\n");
      }else{
        printf("semphr send to task2 fail\n");
      }
   }
    osDelay(20);
  }
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
