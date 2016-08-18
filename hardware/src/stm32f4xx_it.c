/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    21-October-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "ioctrl.h"
#include "main.h"


__IO uint32_t TimingDelay=0;
__IO uint16_t lastDigitalVals=0x0000;
__IO uint16_t lastAnalogVal=0x0000;



/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

extern uint16_t adc_convert();
extern void DAC_SetValue(uint16_t value); 

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  extern __IO uint8_t end_of_sample;
  extern uint32_t Sample_Duration;

  // check for digital output events
  uint16_t numDigitalEvents = io_getNumDigitalOut();
  if( numDigitalEvents > 0 )
  {
    ioevent_digital_t* event = io_peakNextDigitalOut();
    if (event->t == TimingDelay)
    {
      GPIOE->ODR = event->regvals;
      io_popNextDigitalOut();
    }
  }

  // check for analog output events
  uint16_t numAnalogEvents = io_getNumAnalogOut();
  if( numAnalogEvents > 0 )
  {
    ioevent_analog_t* event = io_peakNextAnalogOut();
    if (event->t == TimingDelay)
    {
      DAC_SetValue(event->analogval);
      io_popNextAnalogOut();
    }
  }

  // check for maximum sampling time
  if (TimingDelay >= Sample_Duration)
  {
    // end_of_sample = 1;
    // SysTick->CTRL  =  SysTick->CTRL & (~1UL);
    return;
  }
  
  // read digital input
  uint16_t gpio_vals = GPIOC->IDR;
  if (gpio_vals != lastDigitalVals)
  {
    lastDigitalVals = gpio_vals;
    // queue digital value to be sent over UART
    uart_data_t data = {.type='D', .time=TimingDelay, .val=gpio_vals};
    pack_send( (uint8_t*)&data );
  }

  // read analog input
  uint16_t adc_val = adc_convert();
  if (adc_val != lastAnalogVal)
  {
    lastAnalogVal = adc_val;
    // queue analog value to be sent over UART
    uart_data_t data = {.type='A', .time=TimingDelay, .val=adc_val};
    pack_send( (uint8_t*)&data );
  }

//=====increase local time=====
  TimingDelay++;
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
