all: bin/nunchuck-usb

arduino-serial-lib.o: src/arduino-serial-lib.c
	gcc -o arduino-serial-lib.o -c src/arduino-serial-lib.c

arduino-listen-clicker.o: src/arduino-listen-clicker.c
	gcc -o arduino-listen-clicker.o -c src/arduino-listen-clicker.c

bin/nunchuck-usb: arduino-serial-lib.o arduino-listen-clicker.o
	gcc -o nunchuck-usb arduino-serial-lib.o arduino-listen-clicker.o

clean:
	rm arduino-serial-lib.o arduino-listen-clicker.o
	mv nunchuck-usb bin/nunchuck-usb
