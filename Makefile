SRC=$(wildcard src/*.c)
OBJS=$(patsubst %.c,%.o, $(SRC))
LINKEROPTS = -lhidapi-hidraw

all: nunchuk-usb

 
#bin/arduino-serial-lib.o: src/arduino-serial-lib.c
#	gcc -o bin/arduino-serial-lib.o -c src/arduino-serial-lib.c
#
#bin/arduino-listen-clicker.o: src/arduino-listen-clicker.c
#	gcc -o bin/arduino-listen-clicker.o -c src/arduino-listen-clicker.c
#
nunchuk-usb: $(OBJS) 
	gcc -o nunchuk-usb $(OBJS) $(LINKEROPTS)

clean:
	rm src/*.o bin/nunchuk-usb
