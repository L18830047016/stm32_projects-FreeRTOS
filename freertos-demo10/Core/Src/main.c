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
#include "semphr.h"
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

//低优先级任务配置参数
#define LOW_PRIO 3
#define LOW_STACK_SIZE 128
TaskHandle_t low_Handler;
void Low_Task(void *pvParameters);

//中优先级任务配置参数
#define MID_PRIO 4
#define MID_STACK_SIZE 128
TaskHandle_t mid_Handler;
void Mid_Task(void *pvParameters);

//高优先级任务配置参数
#define HIGH_PRIO 5
#define HIGH_STACK_SIZE 128
TaskHandle_t high_Handler;
void High_Task(void *pvParameters);

//信号量句柄(互斥量)
SemaphoreHandle_t semaphore=NULL;

//high task和mid task双双osDelay(即阻塞)时，low task才能执行
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
  //通过临界区确保业务创建成功
  taskENTER_CRITICAL();
  //创建信号量（互斥量）
  semaphore=xSemaphoreCreateMutex();
  if(semaphore==NULL){
    printf("%s\n","semphr create fail");
  }else{
    printf("%s\n","semphr create success");
   
      xTaskCreate(Low_Task,"Low_Task",LOW_STACK_SIZE,NULL,LOW_PRIO,&low_Handler);
      xTaskCreate(Mid_Task,"Mid_Task",MID_STACK_SIZE,NULL,MID_PRIO,&mid_Handler);
      xTaskCreate(High_Task,"High_Task",HIGH_STACK_SIZE,NULL,HIGH_PRIO,&high_Handler);
      printf("all task create success");
    }
  vTaskDelete(NULL); //删除启动任务
  taskEXIT_CRITICAL();//退出临界区
}

void Low_Task(void *pvParameters){
  BaseType_t res=pdFALSE;
  while(1){
    printf("low task get semphr,low task's curr_prio is %ld\n",uxTaskPriorityGet(low_Handler));
    //得不到信号量，阻塞态
    res=xSemaphoreTake(semaphore,portMAX_DELAY);
    if(res==pdTRUE){
      printf("low task get semph success\n");
      //点灯
      HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET);
      //模拟耗时操作
      for(uint32_t i=0;i<100000;i++){
        taskYIELD();//发起任务调度，增加当前任务被切的概率
      }
      //打印优先级
       printf("during low task running ,low task's curr_prio is %ld\n",uxTaskPriorityGet(low_Handler));
      //任务结束后，释放资源
      printf("low task end,realse samphr\n");
      xSemaphoreGive(semaphore);//释放信号量
      //释放信号量后，当前任务仍可能继续运行（除非被更高优先级任务抢占）
      //实际情况是high task立即得到信号量，将low task抢占，low task进入就绪态
      //再次运行时，从此刻开始
      printf("after low task realse semphr,low task's curr_prio is %ld\n",uxTaskPriorityGet(low_Handler));
    }else{
      printf("low task get semph fail\n");
    }
  osDelay(10);
  }
}

void Mid_Task(void *pvParameters){

  while(1){
    printf("mid task running,turn off the LED\n");
     HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
     for(uint32_t i=0;i<100000000;i++){
      //模拟耗时操作
     }
     printf("mid task end\n");
    osDelay(200);
  }
}

void High_Task(void *pvParameters){
  BaseType_t res=pdFALSE;
  while(1){
    printf("high task try to get semphr\n");
    res=xSemaphoreTake(semaphore,portMAX_DELAY);
    if(res==pdTRUE){
      printf("high task get semph success\n");
      
      //任务结束后，释放资源
      printf("high task end,realse samphr\n");
      xSemaphoreGive(semaphore);//释放信号量
    }else{
      printf("high task get semph fail\n");
    }
    osDelay(1000);//进入阻塞态
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
