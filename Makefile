CC = gcc

ifdef CROSS_COMPILE
export PATH := $(PATH):/home/meenakshi/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/bin
CC = aarch64-none-linux-gnu-gcc
else
CC = gcc
endif

all: writer 

writer: writer.c
	$(CC) writer.c -o writer

clean:
	rm -f writer		
