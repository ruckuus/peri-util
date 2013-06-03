#
# Makefile for printer test application.
#

TARGET_CROSS=/usr/local/arm/4.2.2-eabi/usr/bin/arm-linux-

CC=$(TARGET_CROSS)gcc
CFLAGS=-W -Wall

all:cash_drawer cut_paper

cash_drawer:
	$(CC) $(CFLAGS) cash_drawer.c -o cash_drawer

cut_paper:
	$(CC) $(CFLAGS) cut_paper.c -o cut_paper
clean:
	rm -f test_printer *.c~ Makefile~ cut_paper cash_drawer

