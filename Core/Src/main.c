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

volatile float next_scale_val;
volatile bool update_pending;



void set_phase(float scale, uint8_t dead_time){

    uint16_t phase_duration = (ARR_VAL-scale) - dead_time;//the scale_val in this equation cotrolls overall stimulation (ARR/2 is the max value)

    //output registers
    TIM1->CCR1 = ARR_VAL - (phase_duration/2); //start +phase
    TIM1->CCR2 = (phase_duration/2); //end +phase
    TIM1->CCR3 = (phase_duration/2) + dead_time; //start -phase
    TIM1->CCR4 = phase_duration + (phase_duration/2) + dead_time; //end -phase
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
  TIM2_Init();

  GPIO_Init();

  ADC1_Init();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  float scale_val = 100;

  int data;//adc input data
  float V;//volts
  float mA;//milliamps

  float target_mA = 25.0;
  float error;
  float pid_p;
  float pid_i = 0.0;
  float pid_d;

  float prev_error;

  float max_step = 10.0f;

  uint32_t last = TIM2->CNT;
  uint32_t now;
  uint32_t diff;
  float dt;



  //not fully tuned yet
  float P = 0.5;
  float I = 0.0;
  float D = 0.0;

  float correction_val;



  while (1)
  {



	  //ADC reading and comversions
	  if (ADC1->SR & ADC_SR_JEOC) {
	      ADC1->SR &= ~ADC_SR_JEOC;

	      data = ADC1->JDR1;

	      V = (float)data * 3.3f / 4095.0f;
	      mA = V / 100 * 1000;


		  //delta time calculations
		  now = TIM2->CNT;

		  diff = (now >= last) ? (now - last)
		                       : (0xFFFFFFFF - last + now + 1);

		  dt = (float)diff / 1000000; // seconds (if 1 MHz timer)
		  last = now;





		  //pid error correction
		  error = target_mA - mA;



		  pid_p = error * P;

		  pid_i = ((0.25) * pid_i + error * dt) * I;//will "leak" to 0 overtime if error is small (requires extra tuning)

		  pid_d = ((error - prev_error) / dt) * D;


		  correction_val = pid_p + pid_i + pid_d;

		  if (correction_val > max_step) correction_val = max_step;
		  if (correction_val < -max_step) correction_val = -max_step;

		  scale_val += correction_val;

		  if(scale_val < 0.0){
			  scale_val = 0.0;
		  }

		  if(scale_val > (float)TIM1->ARR/2){
			  scale_val = (float)TIM1->ARR/2;
		  }



		  next_scale_val = scale_val;
		  update_pending = true;



		  prev_error = error;

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

    set_phase(2, 15);//divisor, deadtime


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

    TIM1->DIER |= TIM_DIER_UIE;

    //start tieme
	TIM1->CR1 |= TIM_CR1_CEN;

	NVIC_SetPriority(TIM1_CC_IRQn, 0);
	NVIC_EnableIRQ(TIM1_CC_IRQn);
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

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

void TIM1_UP_TIM10_IRQHandler(void)
{
    if (TIM1->SR & TIM_SR_UIF) {
        TIM1->SR &= ~TIM_SR_UIF;

        if (update_pending) {
            set_phase(next_scale_val, 15);
            update_pending = false;
        }
    }
}

static void TIM2_Init(void){

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	(void)RCC->APB1ENR;//confirm the write


	TIM2->PSC = 83;    // divide 84MHz → 1MHz
	TIM2->ARR = 0xFFFFFFFF;//max value

    TIM2->EGR |= TIM_EGR_UG;





	//TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_CEN;


	//NVIC_SetPriority(TIM2_IRQn, 0);
	//NVIC_EnableIRQ(TIM2_IRQn);
}


/*
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

*/

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
