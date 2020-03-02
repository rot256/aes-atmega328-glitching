#include <avr/io.h>
#include <stdint.h>
#include <string.h>

#include "aes.h"

#define UART_BAUD 9600
#define BLINK_DELAY_MS 10

char HEX[] = {
    '0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
};

// secret AES key
uint8_t key[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

void uart_init(void) {
    UBRR0 = (F_CPU / (16UL * UART_BAUD)) - 1;
    UCSR0B = _BV(TXEN0) | _BV(RXEN0);
}

void uart_putchar(char c) {
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

void uart_putstr(char *s) {
    while (*s) {
        uart_putchar(*s);
        s++;
    }
}

void uart_hex(uint8_t c) {
    uart_putchar(HEX[c >> 4]);
    uart_putchar(HEX[c & 0xf]);
}

__attribute__((optimize("unroll-loops")))
int main (void) {
    uint8_t pt[AES_BLOCKSIZE];
    aes_expanded_key_t ekey;

    // setup serial
    uart_init();

    // set pin 5 of PORTB for output
    DDRB |= _BV(DDB5);

    // set pin 5 high to turn led on */
    PORTB |= _BV(PORTB5);

    // expand AES key
    aes_expand(&ekey, key);

    // repeatedly encrypt same plaintext
    uart_putstr("\n\r");
    while(1) {
        // pt <- 0^16
        memset(pt, 0, sizeof pt);

        // pt <- AES128(key, 0^B)
        aes_encrypt(pt, &ekey);

        // print(pt)
        for (uint8_t i = 0; i < sizeof pt; i++)
            uart_hex(pt[i]);

        uart_putchar('\n');
        uart_putchar('\r');
    }
}
