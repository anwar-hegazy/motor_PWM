MEGA=328p
CC=/usr/bin/avr-gcc
CFLAGS=-g -Os -Wall -mmcu=atmega$(MEGA)
OBJ2HEX=/usr/bin/avr-objcopy
TARGET=motor_PWM
PROG=/usr/bin/avrdude
PROGDEVICE=/dev/ttyACM0

program : $(TARGET).hex
	$(PROG) -c avrisp2 -p m$(MEGA) -P $(PROGDEVICE) -e
	$(PROG) -c avrisp2 -p m$(MEGA) -P $(PROGDEVICE) -U flash:w:$(TARGET).hex

%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f *.hex *.obj *.o
