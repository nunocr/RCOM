#!Transmit an image .gif through the serial port
serialCom: main.c link_layer.c link_layer.h application_layer.c application_layer.h
	gcc -Wall main.c link_layer.c application_layer.c -o serialCom 

debug: main.c link_layer.c link_layer.h
	gcc -g -Wall main.c link_layer.c application_layer.c -o serialCom

clean:
	rm -f serialCom
