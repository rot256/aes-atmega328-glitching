
victim.o: victim.c
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o victim.o victim.c

aes.o: aes.c aes.h
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o aes.o aes.c

victim: victim.o aes.o
	avr-gcc -mmcu=atmega328p victim.o aes.o -o victim

victim.hex: victim
	avr-objcopy -O ihex -R .eeprom victim victim.hex

flash: victim.hex
	avrdude -F -V -c arduino -p m328p -P /dev/ttyUSB0 -b 57600 -U flash:w:victim.hex

clean:
	rm -f *.o
	rm -f *.hex
	rm -f victim

.PHONY: flash clean
