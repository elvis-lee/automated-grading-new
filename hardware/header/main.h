#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"
#include <stdint.h>
#define ALL_PINS (GPIO_Pin_1 |GPIO_Pin_2 |GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7 |GPIO_Pin_8 |GPIO_Pin_9 |GPIO_Pin_10 |GPIO_Pin_11 |GPIO_Pin_12 |GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 | LED4_PIN | LED5_PIN | LED3_PIN | LED6_PIN) // all pins

#define PACKET_TYPE_DIGITAL ('D')
#define PACKET_TYPE_ANALOG ('A')
#define PACKET_TYPE_PARAM_SAMPLELEN ('L')
#define PACKET_TYPE_PARAM_TOTALPACKET ('P')
#define PACKET_TYPE_RESET_SYSTEM ('R')
#define PACKET_TYPE_ANALOG_READING_ENABLE ('O')
#define PACKET_TYPE_DUT_RESET ('U')

//=====frame and data structure=====
typedef struct __attribute__((packed)){
	uint8_t type;
	uint32_t time;
	uint16_t val;
} uart_data_t;

typedef struct __attribute__((packed)){
  uint8_t start;
  uart_data_t data;
  uint8_t stop;
} uart_frame_t;

//=====setup=====
static void pins_setup(void);
static void pins_setup_DUT(void);
static void adc_configure(void);
uint16_t adc_convert(void);
void pack_send(const uint8_t *data);

#endif // _MAIN_H_




