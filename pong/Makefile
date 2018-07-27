#CC_C = gcc 
CC_C = arm-linux-gnueabihf-gcc 
TARGET = pong
OUTDIR = $(HOME)/cmpt433/public/myApps
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -g
PTHREAD = -pthread

#LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB

all: pong

pong:
	$(CC_C) $(CFLAGS) $(PTHREAD) main.c pong.c ledMatrix.c helper.c joystick.c gpio.c display.c udpserver.c -o $(OUTDIR)/$(TARGET) -lpthread
	
clean:
	rm $(OUTDIR)/$(TARGET)
