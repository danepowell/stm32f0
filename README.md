stm32f0
=======

STM32F0-Discovery board libraries

Currently includes these libraries:
* newlib_syscalls: supplies the platform-specific syscalls (e.g. _exit(), _sbrk(), etc...) necessary for use of newlib
* usart_buffer: implements an interrupt-based USART buffer (a much better alternative to the locking USART operations in the StdPeriph library)

