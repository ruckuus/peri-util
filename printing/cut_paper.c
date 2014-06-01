#include <unistd.h>
#include "peri_util.h"

int cut_paper(int fd)
{
	char cut[2] = {0x1b, 0x69};

	if (fd < 0) {
		return -1;
	}

	return write(fd, cut, 2);
}
