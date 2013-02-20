#ifndef USART_H
#define USART_H
#ifdef __cplusplus
extern "C" {
#endif
int getchar(void);
void put_char(int);
void USART2_Interrupt(void);
#ifdef __cplusplus
}
#endif
#endif
