#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "peri_util.h"

#define PDEVICE "/dev/usb/lp0"
#define CUT_PAPER 1
#define OPEN_CASH_DRAWER 2

int do_action(int fd, int action) {
  switch(action) {
    default:
    case CUT_PAPER:
      return cut_paper(fd);
      break;
    case OPEN_CASH_DRAWER:
      return open_cash_drawer(fd);
      break;
  }
}

void show_usage(void) {
  printf("1. Cut paper\n2. Open cash drawer\n");
}

int main(int argc, char **argv)
{
  int fd;
  int action;

  if (argc <= 1) {
    show_usage();
    return -1;
  }

  action = atoi(argv[1]);

	fd = open(PDEVICE, O_RDWR);
  if (fd < 0) {
    return -1;
  }

  return do_action(fd, action);
}
