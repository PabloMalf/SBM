#include "main.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif

#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

#ifdef RTE_CMSIS_RTOS2_RTX5
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount ());
  }

  /* If Kernel is not running wait approximately 1 ms then increment 
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}

#endif

static void SystemClock_Config(void);
static void Error_Handler(void);
static void pin_config(void);
static void init_Timer(void);

TIM_HandleTypeDef tim2;
TIM_HandleTypeDef tim3;


int main(void){
  HAL_Init();
  SystemClock_Config();
  SystemCoreClockUpdate();

  /* App */
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	pin_config();
	init_Timer();
	
	/* App */ 
	
#ifdef RTE_CMSIS_RTOS2
  osKernelInitialize ();

  /* Init Threads */
	
	
	
	/* Init Threads */

  osKernelStart();
#endif

  while(1){
		
	}
}

static void SystemClock_Config(void){
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
	
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
    Error_Handler();
  }
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK){
    Error_Handler();
  }
  if (HAL_GetREVID() == 0x1001){
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

static void Error_Handler(void){
  while(1){}
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){ 
  while (1){}
}
#endif

static void pin_config(void){
	static GPIO_InitTypeDef GPIO_InitStruct;
	//PB11 AF1
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//PB0 AF2
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void init_Timer(void){
	static TIM_OC_InitTypeDef TIM_Channel_InitStruct;
	static TIM_IC_InitTypeDef TIM_Channel_InitStruct_;
	
	//TIM2 CH4 OC-Mode
	__HAL_RCC_TIM2_CLK_ENABLE();
	tim2.Instance = TIM2;
	tim2.Init.Prescaler = 20999;
	tim2.Init.Period = 1;
	HAL_TIM_OC_Init(&tim2);
	
	TIM_Channel_InitStruct.OCMode = TIM_OCMODE_TOGGLE;
	TIM_Channel_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_Channel_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_OC_ConfigChannel(&tim2, &TIM_Channel_InitStruct, TIM_CHANNEL_4);
	HAL_TIM_OC_Start(&tim2, TIM_CHANNEL_4);
	
	
	//TIM3 CH3 IC-Mode	
	__HAL_RCC_TIM3_CLK_ENABLE();
	tim3.Instance = TIM3;
	tim3.Init.Prescaler = 0;
	tim3.Init.Period = 65535;
	HAL_TIM_Base_Init(&tim3);
	HAL_TIM_Base_Start_IT(&tim3);
	
	TIM_Channel_InitStruct_.ICPolarity = TIM_ICPOLARITY_RISING;
	TIM_Channel_InitStruct_.ICSelection = TIM_ICSELECTION_DIRECTTI;
	TIM_Channel_InitStruct_.ICPrescaler = TIM_ICPSC_DIV1;
	TIM_Channel_InitStruct_.ICFilter = 0;
	HAL_TIM_IC_ConfigChannel(&tim3, &TIM_Channel_InitStruct_, TIM_CHANNEL_3);
	HAL_TIM_IC_Start_IT(&tim3, TIM_CHANNEL_3);
	
}
