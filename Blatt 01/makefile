.PHONY: all

all: hw asc rotup hd

hw: hw.c
	gcc -Wall -o hw hw.c

asc: asc.c
	gcc -Wall -o asc asc.c

rotup: rotup.c
	gcc -Wall -o rotup rotup.c

hd: hexdump.c hd.c
	gcc -Wall -o hd hexdump.c hd.c

