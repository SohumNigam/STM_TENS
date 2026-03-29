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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
static void TIM1_Init(void);
static void TIM2_Init(void);
static void GPIO_Init(void);
static void ADC1_Init(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



volatile uint8_t freq;
volatile uint32_t ARR_VAL;
volatile uint8_t ARR_DIV;




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

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  TIM1_Init();
  //TIM2_Init();

  GPIO_Init();

  ADC1_Init();



  void set_phase(float scale_val, uint8_t dead_time){

      uint16_t phase_duration = (ARR_VAL/scale_val) - dead_time;//the 2 in this equation cotrolls overall stimulation (2 is the max value)

      //output registers
      TIM1->CCR1 = ARR_VAL - (phase_duration/2); //start +phase
      TIM1->CCR2 = (phase_duration/2); //end +phase
      TIM1->CCR3 = (phase_duration/2) + dead_time; //start -phase
      TIM1->CCR4 = phase_duration + (phase_duration/2) + dead_time; //end -phase
  }




  /* USER CODE END 2 */

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
      RCC->CR |= RCC_CR_HSION;
      while(!(RCC->CR & RCC_CR_HSIRDY));

      RCC->PLLCFGR =
          (16 << RCC_PLLCFGR_PLLM_Pos) |
          (336 << RCC_PLLCFGR_PLLN_Pos) |
          (1 << RCC_PLLCFGR_PLLP_Pos) |     // /4
          RCC_PLLCFGR_PLLSRC_HSI |
          (2 << RCC_PLLCFGR_PLLQ_Pos);

      RCC->CR |= RCC_CR_PLLON;
      while(!(RCC->CR & RCC_CR_PLLRDY));

      FLASH->ACR |= FLASH_ACR_LATENCY_2WS;

      RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
      RCC->CFGR |= RCC_CFGR_SW_PLL;

      while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
  /* USER CODE BEGIN MX_GPIO_Init_2 */

static void ADC1_Init(void){

	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	ADC1->CR1 &= ~ADC_CR1_RES;//12 bit resolution

	ADC1->JSQR &= ~ADC_JSQR_JL;

	ADC1->JSQR = (1 << ADC_JSQR_JSQ1_Pos); // Channel 1

	ADC1->SMPR2 &= ~ADC_SMPR2_SMP1;
	ADC1->SMPR2 |= ADC_SMPR2_SMP1_0;


	ADC1->CR2 |= ADC_CR2_JEXTEN_0; // rising edge

	ADC1->CR2 &= ~ADC_CR2_JEXTSEL;
	ADC1->CR2 |= ADC_CR2_JEXTSEL_0;//TIM1_TRGO event



	ADC1->CR2 |= ADC_CR2_ADON;

	for (int i=0; i<100000000000; i++){}

}



static void GPIO_Init(void){

	//clock initializations
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;



	//configre pin as alternate function
	//PA7
	GPIOA->MODER |= GPIO_MODER_MODER7_0;
	GPIOA->MODER &= ~GPIO_MODER_MODER7_1;
	//PA8
	GPIOA->MODER |= GPIO_MODER_MODER8_0;
	GPIOA->MODER &= ~GPIO_MODER_MODER8_1;

	//PA0 analog input
	GPIOA->MODER |= GPIO_MODER_MODER0_0;
	GPIOA->MODER |= GPIO_MODER_MODER0_1;

	//Set output type as push-pull
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT7|GPIO_OTYPER_OT8); //PA7 AND PA8

	//no pull up or pull down
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD7|GPIO_PUPDR_PUPD8|GPIO_PUPDR_PUPD0);

	//output speed max
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED7|GPIO_OSPEEDR_OSPEED8;


}

  /* USER CODE END MX_GPIO_Init_2 */

/* USER CODE BEGIN 4 */

static void TIM1_Init(void){

	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	(void)RCC->APB2ENR;


    TIM1->PSC = 8399;       // Prescaler

	freq = 50;

	ARR_VAL  = (84000000 / (TIM1->PSC + 1) / freq) - 1;


    //TIM1->CR1 |= TIM_CR1_ARPE;       //Auto reload preload
    TIM1->ARR = ARR_VAL;


    //phase_duration cant be grater than (ARR_VAL/2)
    //phase duration = 25% by default (not accounting for deadtime)

    set_phase(4, 15);//divisor, deadtime


    TIM1->EGR |= TIM_EGR_UG; // Generate an update event to load registers


    TIM1->CR2 &= ~TIM_CR2_MMS;//CLEAR
    TIM1->CR2 |= TIM_CR2_MMS_1;//triggers TRGO on update event

    //enable preload
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
    TIM1->CCMR1 |= TIM_CCMR1_OC2PE;
    TIM1->CCMR2 |= TIM_CCMR2_OC3PE;
    TIM1->CCMR2 |= TIM_CCMR2_OC4PE;


    //enabable interrupts
    TIM1->DIER |= TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE;


	TIM1->CR1 |= TIM_CR1_CEN;

	NVIC_SetPriority(TIM1_CC_IRQn, 0);
	NVIC_EnableIRQ(TIM1_CC_IRQn);

}


void TIM1_CC_IRQHandler(void)
{
    if (TIM1->SR & TIM_SR_CC1IF){
        TIM1->SR &= ~TIM_SR_CC1IF;  // clear flag

        GPIOA->ODR |= GPIO_PIN_7;

    }

    if (TIM1->SR & TIM_SR_CC2IF){
        TIM1->SR &= ~TIM_SR_CC2IF;

        GPIOA->ODR &= ~GPIO_PIN_7;

    }

    if (TIM1->SR & TIM_SR_CC3IF){
            TIM1->SR &= ~TIM_SR_CC3IF;

            GPIOA->ODR |= GPIO_PIN_8;

    }


    if (TIM1->SR & TIM_SR_CC4IF){
                TIM1->SR &= ~TIM_SR_CC4IF;

                GPIOA->ODR &= ~GPIO_PIN_8;

    }
}

static void TIM2_Init(void){

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	(void)RCC->APB1ENR;//confirm the write


	TIM2->PSC = 83;    // divide 84MHz → 1MHz
	TIM2->ARR = 10;

    TIM2->EGR |= TIM_EGR_UG;





	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_CEN;


	NVIC_SetPriority(TIM2_IRQn, 0);
	NVIC_EnableIRQ(TIM2_IRQn);
}

bool MAIN_OUTPUT = 1;

void TIM2_IRQHandler(void){
    if (TIM2->SR & TIM_SR_UIF){
        TIM2->SR &= ~TIM_SR_UIF;

        if(MAIN_OUTPUT){
        	TIM1->BDTR &= ~TIM_BDTR_MOE;
        	MAIN_OUTPUT = 0;
        }else{
        	TIM1->BDTR |= TIM_BDTR_MOE;
        	MAIN_OUTPUT = 1;
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
