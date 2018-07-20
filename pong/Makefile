all:
	gcc -Wall pong.c -o pong -lncurses

clean:
	rm pong

#CC_C = gcc 
CC_C = arm-linux-gnueabihf-gcc 
TARGET = pong
OUTDIR = $(HOME)/cmpt433/public/myApps
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -g
PTHREAD = -pthread

LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB

all: pong

pong:
	$(CC_C) $(CFLAGS) $(PTHREAD) pong.c -o $(OUTDIR)/$(TARGET) $(LFLAGS) -lpthread -lasound
	
clean:
	rm $(OUTDIR)/$(TARGET)
