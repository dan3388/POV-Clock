#include "UART.h"
#include <cctype>  // For `toupper`



//Constructor
DUART::DUART() : txHead(0), txTail(0), txBusy(false), rxIndex(0) {}

//UART0 Initialization
void DUART::Init() {
    // Enable clocks for UART0 & PORTA (PTA1 = TX, PTA2 = RX)
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

    // Set UART0 clock source to PLL/2 
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);

    // Configure 9600 baud rate
    uint16_t baud_divisor = (41940000 / (16 * 9600));  
    UART0->BDH = (baud_divisor >> 8) & 0x1F;
    UART0->BDL = baud_divisor & 0xFF;
    UART0->C4 = UART0_C4_OSR(15); // OSR = 16

    // Enable TX, RX, and RX interrupt
    UART0->C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_RIE_MASK;

    // Configure PTA1 (TX) and PTA2 (RX) for UART0
    PORTA->PCR[1] = PORT_PCR_MUX(2);
    PORTA->PCR[2] = PORT_PCR_MUX(2);

    // Enable UART0 interrupt in NVIC
    NVIC_EnableIRQ(UART0_IRQn);
}

//Queue a string for transmission
bool DUART::Send(char *buf, unsigned len) {
    if (txBusy || len > UART_BUFFER_SIZE) return false; 

    for (unsigned i = 0; i < len; i++) {
        txBuffer[i] = buf[i];
    }

    txHead = 0;
    txTail = len;
    txBusy = true;

    // Enable TX interrupt
    UART0->C2 |= UART_C2_TIE_MASK;
    return true;
}

//Transmit Interrupt Handler
void DUART::TransmitHandler() {
    if (txHead < txTail) {
        UART0->D = txBuffer[txHead++];
    } else {
        UART0->C2 &= ~UART_C2_TIE_MASK; // Disable TX interrupt
        txBusy = false;
    }
}

//Receive Interrupt Handler
void DUART::ReceiveHandler() {
    char received = UART0->D;

    // Store characters until ENTER is pressed
    if (rxIndex < UART_BUFFER_SIZE - 1 && !(received == 10 || received == 13)) {  
        rxBuffer[rxIndex++] = received;
    } else {
        // Convert received text to uppercase
        for (unsigned i = 0; i < rxIndex; i++) {
            rxBuffer[i] = toupper(rxBuffer[i]);
        }

        // Send back the uppercase text
        Send((char*)rxBuffer, rxIndex);

        // Reset receive buffer index
        rxIndex = 0;
    }
}

// // Converts uint32_t seconds to HH:MM:SS format
void DUART::setBuffer(uint32_t rtcSeconds)
{
	// Get the current time from RTC
        int seconds = rtcSeconds;
        int hours = (seconds / 3600) % 24;
        int minutes = (seconds / 60) % 60;
        int sec = seconds % 60;

        // Format time as HH:MM:SS
        snprintf(buffer, sizeof(buffer), "Time: %02d:%02d:%02d\r\n", hours, minutes, sec);
				Send(buffer, sizeof(buffer)-1);
}