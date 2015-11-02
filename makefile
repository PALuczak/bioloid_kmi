help:
	@echo 'Help details:'
	@echo 'hex: compile hex file'
	@echo 'flash: install hex file'

hex:
	avr-gcc -Os -DF_CPU=8000000UL -mmcu=atmega32u4 -std=c++11 -c bioloid_kmi.cpp
	avr-gcc -DF_CPU=8000000UL -mmcu=atmega32u4  -o bioloid_kmi.elf bioloid_kmi.o
	avr-objcopy -O ihex bioloid_kmi.elf bioloid_kmi.hex
	rm bioloid_kmi.o
	rm bioloid_kmi.elf

flash:
	avrdude -F -V -c arduino -p atmega32u4  -P /dev/ttyACM0 -b 115200 -U flash:w:bioloid_kmi.hex

