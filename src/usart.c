#include "usart.h"
#include "stm32f0xx.h"
#include "stm32f0xx_usart.h"
#include "stm32f0_discovery.h"

#define QUEUE_SIZE 256

struct Queue {
	uint16_t pRD, pWR;
	uint8_t q[QUEUE_SIZE];
};
static int QueueFull(struct Queue *q) {
	return (((q->pWR + 1) % QUEUE_SIZE) == q->pRD);
}
static int QueueEmpty(struct Queue *q) {
	return (q->pWR == q->pRD);
}
static int Enqueue(struct Queue *q, uint8_t data) {
	if (QueueFull(q))
		return 0;
	else {
		q->q[q->pWR] = data;
		q->pWR = ((q->pWR + 1) == QUEUE_SIZE) ? 0 : q->pWR + 1;
	}
	return 1;
}
static int Dequeue(struct Queue *q, uint8_t *data) {
	if (QueueEmpty(q))
		return 0;
	else {
		*data = q->q[q->pRD];
		q->pRD = ((q->pRD + 1) == QUEUE_SIZE) ? 0 : q->pRD + 1;
	}
	return 1;
}

struct Queue UART2_TXq, UART2_RXq;
static int TxPrimed = 0;
int RxOverflow = 0;
void USART2_Interrupt(void) {
	if (USART_GetITStatus(USART2, USART_IT_RXNE ) != RESET) {
		uint8_t data;
		// buffer the data (or toss it if there's no room
		// Flow control will prevent this
		data = USART_ReceiveData(USART2 ) & 0xff;
		if (!Enqueue(&UART2_RXq, data))
			RxOverflow = 1;
	}
	if (USART_GetITStatus(USART2, USART_IT_TXE ) != RESET) {
		uint8_t data;
		/* Write one byte to the transmit data register */
		if (Dequeue(&UART2_TXq, &data)) {
			USART_SendData(USART2, data);
		} else {
			// if we have nothing to send, disable the interrupt
			// and wait for a kick
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			TxPrimed = 0;
		}
	}
}

int getchar(void) {
	uint8_t data;
	if (Dequeue(&UART2_RXq, &data)) {
		return data;
	}
	else {
		return -1;
	}
}

void put_char(int c) {
	while (!Enqueue(&UART2_TXq, c))
		if (!TxPrimed) {
			TxPrimed = 1;
			USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
		}
}
