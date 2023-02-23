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
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}

#endif

static void SystemClock_Config(void);
static void Error_Handler(void);	
	
void LED_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	__HAL_RCC_GPIOB_CLK_ENABLE(); 
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_14 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //modo push pull de salida
	GPIO_InitStruct.Pull = GPIO_NOPULL; //no tiene activación de pull up ni de pull down
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //modo baja frecuencia
	HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
	
}
	
TIM_HandleTypeDef htim7; //define cual es el manejador que vaamos a utilizar
int i;

int main(void)
{
  HAL_Init();

  SystemClock_Config();
  SystemCoreClockUpdate();


#ifdef RTE_CMSIS_RTOS2
  osKernelInitialize ();
  osKernelStart();
#endif
	LED_Init(); // inicializa los led
	
	i = 0;
	
	htim7.Instance = TIM7; //nombre de la instancia con la que trabajamos
	htim7.Init.Prescaler = 8399 ; //valor del preescaler con el que vamos a trabajar-> 8399 parsa que 84MHz/8400=10000Hz
	htim7.Init.Period = 4999; //valor del periodo con el que vamos a trabajar: en este caso queremos llegar a 2Hz (T=500ms)
												   //por lo tanto 10000Hz/5000=2Hz
	
	HAL_NVIC_EnableIRQ(TIM7_IRQn); //habilita el periférico de la IRQ
	__HAL_RCC_TIM7_CLK_ENABLE(); // habilita periférico del timer 7 
	
	HAL_TIM_Base_Init(&htim7); // configura el timer
	HAL_TIM_Base_Start_IT(&htim7); //start el timer (trabajamos en modo interrupcion)

  /* Infinite loop */
  while (1)
  {
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
  RCC_OscInitStruct.PLL.PLLM = 8; // M = 8 para que a la salida le lleguen 168MHz 
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){Error_Handler();}

  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK){Error_Handler();}
  if (HAL_GetREVID() == 0x1001){__HAL_FLASH_PREFETCH_BUFFER_ENABLE();}
}

static void Error_Handler(void){while(1){}}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){while (1){}}
#endif


