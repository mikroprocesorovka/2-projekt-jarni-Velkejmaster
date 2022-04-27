
/* vzorový projekt (sem pište stručný popis projektu)
Domácí úkol na práci s AD převodníkem
*/

#include "stm8s.h"
#include "milis.h"
#include "stdio.h"
#include "stm8s_adc2.h"
#include "spse_stm8.h"

void init_timer(void);
void ADC_init(void);
void pocty (void);
uint16_t time, cas, doba;

uint32_t adc_value0, adc_value1;
int16_t voltage0, voltage1;

void uart_puts(char* retezec);
void uart_putchar(char data);
char text[32];

void main(void){
CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // 16MHz z interního RC oscilátoru
init_milis(); 
GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_SLOW);
ADC_init();
init_timer();
UART1_Cmd(ENABLE);
UART1_Init(115200,UART1_WORDLENGTH_8D,UART1_STOPBITS_1,UART1_PARITY_NO,UART1_SYNCMODE_CLOCK_DISABLE,UART1_MODE_TXRX_ENABLE);

  while (1){

doba=milis();
pocty();

if(doba - time >= 200){
	time = milis();
	if (voltage1>voltage0){
		GPIO_WriteHigh(GPIOC,GPIO_PIN_5);
		} else {
		GPIO_WriteLow(GPIOC,GPIO_PIN_5);
	}
}

TIM2_SetCompare2(voltage1);

if(doba - cas >= 1000){
	cas = milis();
	sprintf(text,"napeti %u.%03u \n\r",(voltage1/1000),(voltage1%1000));
	uart_puts(text);
	}

  }
}

void pocty (void){
adc_value0 = ADC_get(ADC2_CHANNEL_2); //hodnota na děliči
adc_value1 = ADC_get(ADC2_CHANNEL_3); //hodnota na potenciometru
voltage0 = (adc_value0*5000 + 512)/1024; //napětí na děliči
voltage1 = (adc_value1*5000 + 512)/1024; //napětí na potenciometru
}

void ADC_init(void){
ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL2,DISABLE);
ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL3,DISABLE);
ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4);
ADC2_AlignConfig(ADC2_ALIGN_RIGHT);
ADC2_Select_Channel(ADC2_CHANNEL_2);
ADC2_Select_Channel(ADC2_CHANNEL_3);
ADC2_Cmd(ENABLE);
ADC2_Startup_Wait();
}

void uart_putchar(char data){
 while(UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
 UART1_SendData8(data); 
}

void uart_puts(char* retezec){ 
	while(*retezec){ 
		uart_putchar(*retezec);
		retezec++;
	}
}

void init_timer(void){
GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);
TIM2_TimeBaseInit(TIM2_PRESCALER_16,5000-1);
TIM2_OC2Init(TIM2_OCMODE_PWM1,TIM2_OUTPUTSTATE_ENABLE,0,TIM2_OCPOLARITY_HIGH);
TIM2_OC1PreloadConfig(ENABLE);
TIM2_Cmd(ENABLE);
}

// pod tímto komentářem nic neměňte 
#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */                                                
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */                               
  while (1)
  {
  }
}
#endif

