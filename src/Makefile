PREFIX=/usr/local/avr-gcc4/bin

CC = $(PREFIX)/avr-gcc
AS = $(CC) -x assembler-with-cpp
SIZE= $(PREFIX)/avr-size
NM=$(PREFIX)/avr-nm


#/usr/local/avr/bin/avr-nm --size-sort toothfairy.elf   

MCU=attiny2313
CSRCS=main.c pulse_stream.c semaphore.c hex_string.c key.c\
	 led.c state_machine.c key_mapping.c cpld_if.c \
	user_config.c soft_reset.c
ASRCS=
TARGET=toothfairy
FORMAT= ihex

DEPS = $(CSRCS:.c=.d)

OBJS = $(ASRCS:.s=.o) $(CSRCS:.c=.o) 

OBJCOPY=$(PREFIX)/avr-objcopy
OBJDUMP=$(PREFIX)/avr-objdump

CFLAGS = -mmcu=$(MCU) -ffunction-sections -Wall -Os -g -fshort-enums -mcall-prologues #-D__DEBUG__ 
LDFLAGS = -g -Os --ffunction-sections -Wl,--relax,--gc-sections,-Map,$(TARGET).map 
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
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^
	$(SIZE) $@
	$(NM) --size-sort $@  > $(TARGET).size

%.o : %.c 
	$(CC) $(CFLAGS) -c $<

%.s : %.c
	$(CC) -S $(CFLAGS) $<

%.o : %.s
	$(AS) -c $(ASFLAGS)  $< -o $@


%.d : %.c
	set -e; $(CC) $(CFLAGS) -MM $(INCLUDES) $(LINKFLAGS) $< | \
	sed -e 's@^\(.*\)\.o:@./\1.d ./\1.o:@' > $@


ifeq (, $(findstring $(MAKECMDGOALS), clean ))
-include $(DEPS)
endif


clean:
	-rm -f $(OBJS) $(DEPS)
	-rm -f $(TARGET).map 
	-rm -f $(TARGET).elf 
	-rm -f $(TARGET).lst 
	-rm -f $(CSRCS:.c=.s)
	-rm -f $(TARGET).hex
	-rm -f $(TARGET).eep
	-rm -f $(TARGET).size
	-rm -f *~


	
