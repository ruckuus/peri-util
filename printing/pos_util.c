#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
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

int main(void)
{
  int fd;
	fd = open(PDEVICE, O_RDWR );
  if (fd < 0) {
    return -1;
  }

  if (do_action(fd, CUT_PAPER) < 0) {
    return -1;
  }

  return 0;
}
