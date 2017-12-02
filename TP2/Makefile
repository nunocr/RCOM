#!Transmit an image .gif through the serial port
downapp: clientFTP.c clientFTP.h
	gcc -Wall clientFTP.c -o downapp 

debug: main.c clientFTP.c clientFTP.h
	gcc -g -Wall clientFTP.c -o downapp

clean:
	rm -f downapp

