#ifndef SYS_INIT_H_
#define SYS_INIT_H_

#include "stm32f10x_conf.h"
#include "tasks.h"

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure2;

#define SystemCoreClock  72000000
#define LED_PIN_GREEN                   GPIO_Pin_5
#define LED_GPIO_PORT                   GPIOB
#define LED_GPIO_CLK                    RCC_APB2Periph_GPIOB  

static int counter;

static int current;


void LEDToggle(void);

/*
 * MySystemInit is used to set default system initializations and configure system * clock to 72MHz.
*/
void init_system_clk(void);

/*
 * Initializes pin PB5 which is connected to the green LED. 
 * Port mode is push-pull output.
 */
void init_blink(void);

/*
 * Initializes pins connected to motors and configures respective timers 
 * to output PWM signals.
 */
void init_motors(void);

#endif
