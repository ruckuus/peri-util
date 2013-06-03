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
	char pulse[5] = {0x1b, 0x70, 0x00, 0x25, 0x25};

	fd = open(PDEVICE, O_RDWR );
	if (fd < 0) {
		perror("DEVICE");
	}

	printf("Send Pulse\n");
	write(fd, pulse, 5);

	return 0;
}
