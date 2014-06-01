#
# Makefile for printer test application.
#

TARGET_CROSS=/usr/local/arm/4.2.2-eabi/usr/bin/arm-linux-

#CC=$(TARGET_CROSS)gcc
CC=cc
CFLAGS=-W -Wall
DEPS=peri_util.h
OBJS=pos_util.o cash_drawer.o cut_paper.o

all:pos_util

%.o: %.c $(DEPS) 
	$(CC) $(CFLAGS) -c -o $@ $<

pos_util:$(OBJS) 
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f  *~ Makefile~  *.o pos_util

