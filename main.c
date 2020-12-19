#include "stm32f10x.h"                  // Device header

int main(void)
{	
	PWR->CR |=PWR_CR_CSBF;		//clear standby flag
	PWR->CSR |= ~PWR_CSR_EWUP;		//disnable wakeup pin 
	PWR->CR |= PWR_CR_CWUF;			/* Clear Wake-up flag */
	RCC->APB2ENR |=0x04;	//enable clock for gpio a input
	GPIOA->CRL &= 0xFFFFFFF0;	//CLEAR a0 state config
	GPIOA->CRL |=0x00000008;	//set as input pullupdown
	__disable_irq();		//diable interrupts
	//here we set up the interrupt:
	AFIO->EXTICR[0]=0x00;		//pa0 as interrupt
	EXTI->IMR |=0x1;	//interrupt enable on line 0
	EXTI->RTSR |=0x1;		//ENABLE rising triger on interrupt line0 
	NVIC_EnableIRQ(EXTI0_IRQn); 		//enables interrupt set above at line 0
	__enable_irq();		//ENABLE interrupts on the core
	
	RCC->APB2ENR |=0x10;	//enable apb2 clock for gpioc output

	GPIOC->CRH &=0xFF0FFFFF;	//clear settings for c13
	GPIOC->CRH |=0x00300000;	//c13 as output push pull 50mhz
	GPIOC->ODR |=0x00002000;	
	GPIOC->CRH &= 0xF0FFFFFF;	//CLEAR_BIT c14 status settings
	GPIOC->CRH |=0x08000000;	//input push pullup pulldown c14

	while(1)
	{
		
		if((GPIOC->IDR & 0x00004000)>>14)
		{
			
			GPIOC->ODR |=0x00002000;//off
			PWR->CSR |= PWR_CSR_EWUP;		//enable wakeup pin 
			PWR->CR |= PWR_CR_PDDS;			// standby register
			PWR->CR |= PWR_CR_LPDS;			//DISABLE internal regulator during sleep time
			SCB->SCR |= SCB_SCR_SLEEPDEEP;
			__force_stores();
			__WFI();
		}else{
			GPIOC->ODR &=0xFFFFDFFF;	//set low so enable c13 led
						}
	}
}

void EXTI0_IRQHandler()
{
		EXTI->PR |=0x1;		//delete pending irq
}