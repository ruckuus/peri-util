#include <unistd.h>
#include "peri_util.h"

int open_cash_drawer(int fd)
{
	char pulse[5] = {0x1b, 0x70, 0x00, 0x25, 0x25};

	if (fd < 0) {
    return -1;
	}

	return write(fd, pulse, 5);
}
