DEVICE := atmega32u4
CLOCK := 16000000UL
SOURCES := $(wildcard *.cpp)
OBJECTS := ${SOURCES:.cpp=.o}
DEPENDENCIES := $(wildcard *.h)
PROGRAMMER := -F -V -c arduino -P /dev/ttyACM0 -b 115200
FUSES := -U lfuse:w:0x64:m -U hfuse:w:0xdd:m -U efuse:w:0xff:m
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-g++ -Wall -std=c++11 -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

.PHONY: all clean distclean

help:
	@echo 'Help details:'
	@echo 'elf: compile elf file'
	@echo 'hex: compile hex file'
	@echo 'disasm: avr-objdump'
	@echo 'clean: remove created files'
	@echo 'install: install hex file'

all: hex

.c.o: $(DEPENDENCIES)
	$(COMPILE) -c $(SOURCES) -o $(OBJECTS)

.S.o: $(DEPENDENCIES)
	$(COMPILE) -x assembler-with-cpp -c $(SOURCES) -o $(OBJECTS)

.c.s: $(DEPENDENCIES)
	$(COMPILE) -S $(SOURCES) -o $(OBJECTS)

.cpp.s: $(DEPENDENCIES)
	$(COMPILE) -S $(SOURCES) -o $(OBJECTS)

$(OBJECTS): $(DEPENDENCIES)

%.o:%.cpp $(DEPENDENCIES)
	$(COMPILE) -c $< -o $@

elf: $(OBJECTS)
	$(COMPILE) -Wl,-Map,main.map -o main.elf $(OBJECTS)

hex: elf
	rm -f main.hex
	avr-size main.elf
	avr-objcopy -O ihex main.elf main.hex

disasm: elf
	avr-objdump -d main.elf

# untested
#install: all
#	$(AVRDUDE) $(FUSES) -U flash:w:main.hex:i

clean:
	rm -f main.hex main.elf main.map $(OBJECTS)

distclean: clean