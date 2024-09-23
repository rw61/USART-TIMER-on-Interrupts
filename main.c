#include <stm32f4xx.h>
#include <stm32f411xe.h>



#include <stdio.h>
#include <string.h>

#include "delay_on_tim3.c"

static void USART2_Init(void);		 
//static void USART2_Send(char chr);
//static void USART2_Send_String(char* str);
static void CMSIS_USART_Transmit(USART_TypeDef *USART, uint32_t *data, uint16_t Size);
static void GPIO_Init(void); 
static int A[10] = {1,2,3,4,5,6,7,8,9,0}; 
static uint32_t temp = 0;

struct USART_name {
	uint32_t tx_buffer[64]; //Буфер под выходящие данные
	uint32_t rx_buffer[64]; //Буфер под входящие данные
	uint16_t rx_counter; //Счетчик приходящих данных типа uint8_t по USART
	uint16_t rx_len; //Количество принятых байт после сработки флага IDLE
};
static struct USART_name husart2; 






//static void USART_Init(void);
//static void GPIO_Init(void);
//static void USART2_Send(char chr);
//static void USART2_Send_String(char* str);
//static uint32_t temp1 = 0;
//static uint32_t array[10]={0,1,2,3,4,5,6,7,8,9};
//static uint32_t data = 0;
//static uint32_t temp = 0;
//static uint32_t data4Int = 15;
//	
//static uint32_t data2 = 12;
//static char data3;


/////////////////////////ОБРАБОТЧИК ПРЕРЫВАНИЯ////////////////////////////////////////////////////////////////////////
//void USART2_IRQHandler(void){
//	
//	if((USART2->SR & USART_SR_RXNE)!=0){
//	//
//		        temp1 = USART2->DR;                       //получили данные из терминала, например, записали их в переменную и тут же отправили строку(ниже)
//		       for(int i = 0; i <10000; i++){}
//         		USART2_Send_String("Byte received\r\n");
//						//for(int i = 0; i <3000000; i++){}
//						//USART2->DR = data4Int;
//					
//		        USART2->SR &= ~USART_SR_RXNE;        //очистили флаг , задержка
//						for(int i = 0; i <10000; i++){}
//						
//							if((USART2->SR &USART_SR_TC)!=0)              //если данные переданные, передаём другие , очищаем флаг
//							{
//								USART2->DR = temp1;
//								for(int i = 0; i <100000; i++){}
//								USART2_Send_String("\r\nByte sent\r\n\r\n");
//								USART2->SR &= ~USART_SR_TC;
//							}
//						}
//}





 void USART2_IRQHandler(void) {
	 
	
	if (READ_BIT(USART2->SR, USART_SR_RXNE)) {
		//Если пришли данные по USART
		husart2.rx_buffer[husart2.rx_counter] = USART2->DR; //Считаем данные в соответствующую ячейку в rx_buffer. ВСЕ ПРИШЕДШИЕ ДАННЫЫЕ ЗАПИСЫВАЕМ В МАССИВ
		husart2.rx_counter++; //Увеличим счетчик принятых байт на 1
						

	}
	if (READ_BIT(USART2->SR, USART_SR_IDLE)) {
		//Если прилетел флаг IDLE
		USART2->DR; //Сбросим флаг IDLE
		husart2.rx_len = husart2.rx_counter; //Узнаем, сколько байт получили
		CMSIS_USART_Transmit(USART2, husart2.rx_buffer, husart2.rx_counter); //Отправим в порт то, что прилетело для проверки.
	 
		husart2.rx_counter = 0; //сбросим счетчик приходящих данных
	}
}

 






int main(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	USART2_Init(); 
	TIM3_Init();
	GPIO_Init();
	__enable_irq();
	NVIC_EnableIRQ (TIM3_IRQn);
	NVIC_EnableIRQ(USART2_IRQn);
	
	
	
	
																			/////////// INTERRUPT MODE ////////////////////
	

//  for(int i = 0; i <8000000; i++){} //ЗАДЕРЖКА 
	while(1){
		       
		
		
		          SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS5);
					    delay_mS(100);
					
							SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR5);
				  		delay_mS(100);
		
			}
	
	  			  


}
	
	

        /////////////////////// ИНИЦИАЛИЗАЦИЯ ///////////////////////////////////////////

static void USART2_Init(void){

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	
	
	USART2->BRR = 0x8B; ////0x683 для 9600; // 0x8B - 115200 baud
	USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TXEIE | USART_CR1_TCIE ;
	
	MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0, GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1); //ALTERNATIVE FUNC PIN2/3 PORTA
	
	GPIOA->AFR[0]	|= (7 << GPIO_AFRL_AFSEL2_Pos) ; // That alt fn is alt 7 for PA2
	GPIOA->AFR[0] 	|= (7 << GPIO_AFRL_AFSEL3_Pos) ; // Alt fn for PA3 is same as for PA2
	
  
	
	
}

static void GPIO_Init(void){
		 
      GPIOA->MODER &= ~GPIO_MODER_MODER5_1;					//moder5 = 01 - general purpouse output mode
			GPIOA->MODER |= GPIO_MODER_MODER5_0; 
	    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






									//////////////////////// ФУНКЦИИ ПЕРЕДАЧИ СИМВОЛА/СТРОКИ /////////////////////////////

//static void USART2_Send(char chr)   
//{
//	while(!(USART2->SR & USART_SR_TC));
//	USART2->DR = chr;
//}

//void USART2_Send_String(char* str)
//{
//	uint8_t i = 0;
//	while(str[i]){
//		USART2_Send(str[i++]);
//	}
//}

////////////////////////функция передачи данных////////////////////

static void CMSIS_USART_Transmit(USART_TypeDef *USART, uint32_t *data, uint16_t Size) {
	for (uint16_t i = 0; i < Size; i++) {
		while (READ_BIT(USART2->SR, USART_SR_TXE) == 0) ; //Ждем, пока линия не освободится
		USART->DR = *data++; //Кидаем данные  
	}	
}

















																			//////// P O L L I N G  M O D E ///////////////
//	USART2->DR = 88;
//	for(int i = 0; i <1000000; i++){} //ЗАДЕРЖКА 
//	USART2_Send_String("OK\r\n");
//	while(1){
		
//     while ((USART2->SR & USART_SR_RXNE) == 0) {}
//			 data = USART2->DR;
//	     // for(i = 0; i <3000000; i++){} //ЗАДЕРЖКА
//		 
//			
// 			USART2->DR = data; 	
//		//	for(i = 0; i <8000000; i++){} //ЗАДЕРЖКА     
//				while((USART2->SR & USART_SR_TC) == 0) {} //ждать окончания передачи
//					USART2->SR &= ~USART_SR_TC;	            //очистить флаг окончания передачи
  // }   



								//////////////////////////////////////////////////////////////////////////////////////////