EXECUTABLE := $(shell basename `pwd`)
SRC = $(shell find . -name '[!.]*.c' -not -path "./contrib/*")

all: *.c *.h
	gcc -Wall -O0 $(SRC) -o $(EXECUTABLE) -lm `pkg-config --libs --cflags libusb-1.0` -D LINUX

clean:
	rm -f $(EXECUTABLE)