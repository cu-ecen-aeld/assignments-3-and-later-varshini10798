CC = gcc

ifdef CROSS_COMPILE
export PATH := $(PATH):/home/meenakshi/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin
CC = aarch64-none-linux-gnu-gcc
else
CC = gcc
endif

all: aesdsocket
default: aesdsocket 

aesdsocket: aesdsocket.c
	$(CC) aesdsocket.c -o aesdsocket

clean:
	rm -f aesdsocket 		
