PREFIX=/usr/local/avr-gcc4/bin
PREFIX=/usr/bin

CC = $(PREFIX)/avr-gcc
AS = $(CC) -x assembler-with-cpp
SIZE= $(PREFIX)/avr-size
NM=$(PREFIX)/avr-nm


#/usr/local/avr/bin/avr-nm --size-sort toothfairy.elf   

MCU=atmega48
CSRCS=main.c scanner.c pulse_stream.c semaphore.c hex_string.c key.c\
	 led.c state_machine.c  \
	user_config.c soft_reset.c scan_mapping.c
ASRCS= active_scanner.S
TARGET=toothfairy
FORMAT= ihex

DEPS = $(CSRCS:.c=.d)

OBJS = $(ASRCS:.S=.o) $(CSRCS:.c=.o) 

OBJCOPY=$(PREFIX)/avr-objcopy
OBJDUMP=$(PREFIX)/avr-objdump

F_CPU=8000000UL

CFLAGS = -mmcu=$(MCU) -ffunction-sections -Wall -O2 -g -fshort-enums -mcall-prologues #-D__DEBUG__ 
CFLAGS += -DF_CPU=$(F_CPU)
ASFLAGS = -mmcu=$(MCU) -ffunction-sections -Wall -O2 -g -fshort-enums -mcall-prologues
ASFLAGS += -DF_CPU=$(F_CPU)
LDFLAGS = -g -O2 -Wl,--relax,--gc-sections,-Map,$(TARGET).map 

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
	$(SIZE) -t $@ >> $(TARGET).size

%.o : %.c 
	$(CC) $(CFLAGS) -c $<

%.s : %.c
	$(CC) -S $(CFLAGS) $<

%.o : %.s
	$(AS) -c $(ASFLAGS)  $< -o $@

%.o : %.S
	$(CC) -c $(ASFLAGS) $< -o $@

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


	
