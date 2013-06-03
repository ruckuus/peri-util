#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PDEVICE "/dev/usb/lp0"

int main(void)
{
	int fd;
	char cut[2] = {0x1b, 0x69};

	fd = open(PDEVICE, O_RDWR );
	if (fd < 0) {
		perror("DEVICE");
	}

	printf("Cut paper\n");
	write(fd, cut, 2);

	return 0;
}
