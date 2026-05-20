/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "cmsis_os.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED0_Pin|LED1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = LED0_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = KEY1_Pin|KEY2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
uint8_t Key_GetKeyNum(void){
  uint8_t key_num = 0;//表示没有按键被按下
  if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == 1){
    //消除抖动
    osDelay(10);
    //等待按键释放
    while(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == 1);
    //消除抖动
    osDelay(10);
    //按键按下并释放完成，返回按键编号
    key_num = 1;
  }
  //检测按键2
  if(HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == 1){
    //消除抖动
    osDelay(10);
    //等待按键释放
    while(HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == 1);
    //消除抖动
    osDelay(10);
    //按键按下并释放完成，返回按键编号
    key_num = 2;
  }
  return key_num;
}

/* USER CODE END 2 */
