/************************************************************************************
 *  File:     main.c
 *  Purpose:  Cortex-M4 main file.
 *            Replace with your code.
 *  Date:     05 July 2013
 *  Info:     If __NO_SYSTEM_INIT is defined in the Build options,
 *            the startup code will not branch to SystemInit()
 *            and the function can be removed
 ************************************************************************************/
#include "main.h"
#include "config_io.h"
#include "core_cm4.h"
#include "stm32f4xx_conf.h"
#include "mp3dec.h"
#include <string.h>
#include "5110.h"
/*********************************************************************/
//USB 	FS Mode 	HS in FS Mode 	Description
//Data + 	PA12 	PB15 	USB Data+ line
//Data – 	PA11 	PB14 	USB Data- line
//ID 	PA10 	PB12 	USB ID pin
//VBUS 	PA9 	PB13 	USB activat
/*********************************************************************/

/*********************************************************************/
// Macros
#define f_tell(fp)		((fp)->fptr)
/*********************************************************************/
/*********************************************************************/
// Variables
volatile uint32_t		time_var1, time_var2, time_var3, time_var4;
volatile uint32_t		petla_main_licznik;
volatile uint32_t		tim3_licznik, tim3_licznik_pomoc;
USB_OTG_CORE_HANDLE		USB_OTG_Core;
USBH_HOST				USB_Host;
RCC_ClocksTypeDef		RCC_Clocks;
volatile int			enum_done = 0, usb_stan = 0;
volatile bool			LCD5110_block_int = 0;
// other variables
static int16_t 			czas_dekodowania;
static int16_t 			czas_dekodowania_pomoc;
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
void main()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_BaseStruct;
	TIM_OCInitTypeDef TIM_OCStruct;

	// SysTick interrupt each 1ms
	RCC_GetClocksFreq(&RCC_Clocks);
//	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
	SysTick_Config(SystemCoreClock/1000); //przerwanie co 1ms

 	// init phisical inputs/outputs
	InitializeIOs();

	// Init TIM3
	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* Enable the TIM3 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    // disable timer to be shure
	TIM_Cmd(TIM3, DISABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
		TIM_BaseStruct.TIM_Period = 1084;
		TIM_BaseStruct.TIM_Prescaler = 0;
		TIM_BaseStruct.TIM_ClockDivision = 0;
		TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_BaseStruct);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    // enable timer
	TIM_Cmd(TIM3, ENABLE);

	// Configure nokia5110 lcd
	LCD5110_init();

	// Show info
	LCD5110_block_int=1;
	LCD5110_set_XY(0, 1);
	LCD5110_write_string("Core 446re tst");
	LCD5110_write_string("  Build with  ");
	LCD5110_write_string("       emIDE  ");
	LCD5110_block_int=0;

	// Initialize USB Host Library
	USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_Callbacks);

while (1);{
 }
}

/******************************************************************/
/******************************************************************/
//Called by the SysTick interrupt
void TimingDelay_Decrement(void) {
	if (time_var1) {time_var1--;}
	time_var2++;
	if(time_var2>=250){
		time_var2=0;
		GPIO_ToggleBits(GPIOA,LED);
	}
	time_var3++;
	if(time_var3>=100){
		time_var3=0;
		tim3_licznik_pomoc=tim3_licznik;
		tim3_licznik=0;
	}
	if(czas_dekodowania_pomoc>0){
		czas_dekodowania_pomoc++;
	}
}
/******************************************************************/
/******************************************************************/
//Delay a number of systick cycles (1ms)
void Delay(volatile uint32_t nTime) {
	time_var1 = nTime;
	while(time_var1){};
}
/******************************************************************/
/******************************************************************/
//przerwanie do audio DAC
void TIM3_przerwanie(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		tim3_licznik++;
    }
}
/******************************************************************/
/******************************************************************/
