
#include <stdint.h>

#define P5 5
#define P2 2
#define __IO volatile

typedef struct UART_t{
	__IO uint32_t UART_SR;
	__IO uint32_t UART_DR;
	__IO uint32_t UART_BRR;
	__IO uint32_t UART_CR1;
	__IO uint32_t UART_CR2;
	__IO uint32_t UART_CR3;
	__IO uint32_t UART_GTPR;
}UART_t;

typedef struct RCC_t{
	__IO uint32_t RCC_CR;
	__IO uint32_t RCC_PLLCFGT;
	__IO uint32_t RCC_CFGR;
	__IO uint32_t RCC_CIR;
	__IO uint32_t RCC_AHB1RSTR;
	__IO uint32_t RCC_AHB2RSTR;
	__IO uint32_t res1[2];
	__IO uint32_t RCC_APB1RSTR;
	__IO uint32_t RCC_APB2RSTR;
	__IO uint32_t res2[2];
	__IO uint32_t RCC_AHB1ENR;
	__IO uint32_t RCC_AHB2ENR;
	__IO uint32_t res3[2];
	__IO uint32_t RCC_APB1ENR;
	__IO uint32_t RCC_APB2ENR;
	__IO uint32_t res4[2];
	__IO uint32_t RCC_AHB1LPENR;
	__IO uint32_t RCC_AHB2LPENR;
	__IO uint32_t res5[2];
	__IO uint32_t RCC_APB1LPENR;
	__IO uint32_t RCC_APB2LPENR;
	__IO uint32_t res6[2];
	__IO uint32_t RCC_BDCR;
	__IO uint32_t RCC_CSR;
	__IO uint32_t res7[2];
	__IO uint32_t RCC_SSCGR;
	__IO uint32_t RCC_PLLI2SCFGR;
	__IO uint32_t res8;
	__IO uint32_t RCC_DCKCFGR;
}RCC_t;

typedef struct GPIOx_t{
	__IO uint32_t GPIOx_MODER; 		//0x00
	__IO uint32_t GPIOx_OTYPER;		//0x04
	__IO uint32_t GPIOx_OSPEEDER;	//0x08
	__IO uint32_t GPIOx_PUPDR;		//0x0C
	__IO uint32_t GPIOx_IDR;			//0x10
	__IO uint32_t GPIOx_ODR;			//0x14
	__IO uint32_t GPIOx_BSRR;		//0x18
	__IO uint32_t GPIOx_LCKR;
	__IO uint32_t GPIOx_AFRL;
	__IO uint32_t GPIOx_AFRH;
}GPIOx_t;

void wait_ms(int time){
	for(volatile int i = 0; i < time; i++){
		for(volatile int j = 0; j < 1600; j++);
	}
}

GPIOx_t * const GPIOA = (GPIOx_t *)0x40020000;
GPIOx_t * const GPIOB = (GPIOx_t *)0x40020400;
RCC_t   * const RCC   = (RCC_t   *)0x40023800;
UART_t  * const UART2 = (UART_t  *)0x40004400;

void UART2_init(void){
	// 1. Enable GPIOA Clock
	RCC->RCC_AHB1ENR |= 1;
	// 2. Enable UART2 Clock P17
	RCC->RCC_APB1ENR |= (1 << 17);
	// 3. Configure Alternate Function AF07 for PA2 (UART2_TX) to BIN(0111) = DEC(7) for AF7
	GPIOA->GPIOx_AFRL &= ~(0xF << (P2 * 4));
	GPIOA->GPIOx_AFRL |=  (7 << (P2 * 4));
	// 4. Enable alternate function for PA2 set MODER to 10 to set AF
	GPIOA->GPIOx_MODER &= ~(3 << (P2 * 2));
	GPIOA->GPIOx_MODER |=  (2 << (P2 * 2));

	// 5. Set 9600 Baudrate @ 16MHz -> BRR = USARTDIV -> 16/(Sample * Baud) = 104.1666666 -> Mantissa = BRR[15:4] = 104 = 0x68 | DIV = BRR[3:0] = 0.16666 * 16 ~ 3 = 0x3
	UART2->UART_BRR = ((0x68 << 4) | 0x3);
	// 6. Enable TX CR1[3], Set Word length to 8 CR1[12]
	UART2->UART_CR1 |=  (1 << 3);
	UART2->UART_CR1 &= ~(1 << 12);
	// 7. Set 1 Stop Bit
	UART2->UART_CR2 = 0x0000;
	// 8. Disable fow control
	UART2->UART_CR3 = 0x0000;
	// 9. Enable CR1[13] UART Enable
	UART2->UART_CR1 |= (1 << 13);
}

void UART2_write(int ch){
	// Check if TxDR has been finished shifting to shift register by checking SR[7] TXE Flag
	while(!((UART2->UART_SR & (1 << 7))));
	UART2->UART_DR = (ch & 0xFF);
}

int main(void)
{
	UART2_init();

	for(;;){
		UART2_write('E');
		UART2_write('L');
		UART2_write('A');
		UART2_write('N');
		UART2_write('U');
		UART2_write('R');
		wait_ms(500);
	}
}
