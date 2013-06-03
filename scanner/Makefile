CC=/usr/local/arm/4.2.2-eabi/usr/bin/arm-linux-gcc

DEV=/dev/input/event11

barcode:barcode.c
	$(CC) $(CFLAGS) $< -o $@

debug:
	sudo ./barcode $(DEV)
