CC = avr-gcc
AS = avr-gcc -x assembler-with-cpp

MCU=attiny2313
CSRCS=main.c endpoint.c
ASRCS=
TARGET=cpldprog
FORMAT= ihex


OBJS = $(ASRCS:.s=.o) $(CSRCS:.c=.o) 

OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump

CFLAGS = -g -mmcu=$(MCU) -Wall -Wstrict-prototypes 
ASFLAGS = -mmcu=$(MCU)

.PHONY: all
all: $(CSRCS:.c=.s) $(TARGET).lst $(TARGET).hex $(TARGET).eep 

$(TARGET).lst : $(TARGET).elf
	$(OBJDUMP) -S $^  > $@

$(TARGET).hex : $(TARGET).elf 
	$(OBJCOPY) -R .eeprom -O $(FORMAT) $< $@

$(TARGET).eep : $(TARGET).elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

$(TARGET).elf : $(OBJS)
	$(CC) $(CFLAGS) -o $@ -Wl,-Map,$*.map $^

%.o : %.c 
	$(CC) $(CFLAGS) -Os -c $<

%.s : %.c
	$(CC) -S $(CFLAGS) -Os $<

%.o : %.s
	$(AS) -c $(ASFLAGS)  $< -o $@


clean:
	-rm -f $(OBJS) 
	-rm -f $(TARGET).map 
	-rm -f $(TARGET).elf 
	-rm -f $(TARGET).lst 
	-rm -f $(CSRCS:.c=.s)
	-rm -f $(TARGET).hex
	-rm -f $(TARGET).eep

