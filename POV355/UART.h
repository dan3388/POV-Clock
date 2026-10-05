#include <MKL16Z4.h>
#include <cstdio>

#ifndef DUART_H
#define DUART_H

#define UART_BUFFER_SIZE 100  // Buffer size for TX & RX

class DUART {
public:
    DUART();  // Constructor
    void Init();  // Initialize UART0
    bool Send(char *buf, unsigned len); // Queue a string for TX
    void TransmitHandler(); // Handles UART TX interrupts
    void ReceiveHandler();  // Handles UART RX interrupts
		void setBuffer(uint32_t rtcSeconds); // Converts uint32_t seconds to HH:MM:SS format

private:
    volatile char txBuffer[UART_BUFFER_SIZE]; // TX buffer
    volatile unsigned txHead, txTail;  // TX buffer pointers
    volatile bool txBusy; // Transmission busy flag
		char buffer[30];
    volatile char rxBuffer[UART_BUFFER_SIZE]; // RX buffer
    volatile unsigned rxIndex; // RX buffer index
};

extern DUART g_uart;  // Global UART object

#endif