#CC_C = gcc 
CC_C = arm-linux-gnueabihf-gcc 
TARGET = pong
OUTDIR = $(HOME)/cmpt433/public/myApps
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -g
PTHREAD = -pthread
SOURCES = main.c pong.c helper.c joystick.c gpio.c display.c

#LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB

all: pong_driver

pong_driver:
	$(CC_C) $(CFLAGS) $(PTHREAD) $(SOURCES) driver.c -D MATRIX_DRIVER -o $(OUTDIR)/$(TARGET) -lpthread

pong_user:
	$(CC_C) $(CFLAGS) $(PTHREAD) $(SOURCES) ledMatrix.c -o $(OUTDIR)/$(TARGET) -lpthread

	
clean:
	rm $(OUTDIR)/$(TARGET)
