#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#define KEY_PRESS 1
#define KEY_KEEPING_PRESSED 2

 int main (int argc, char *argv[])
{
	struct input_event ev[64];
	int fd, rd,size = sizeof (struct input_event);
	char name[256] = "Unknown",i;
	char *device = NULL;
	char decode_scancode[]={0,0,1,2,3,4,5,6,7,8,9,0};
 
	if (argv[1] == NULL){
		printf("Please enter path to device\n");
		return 1;
	}
 
	if ((getuid ()) != 0)
		printf ("You must be as root for correct work!\n");
 
	if (argc > 1)
		device = argv[1];
 
	if ((fd = open (device, O_RDONLY)) == -1){
		printf ("Open error of device %s\n", device);
		return 1;
	}
 
	ioctl (fd, EVIOCGNAME (sizeof (name)), name);
//	printf ("Reading From : %s (%s)\n", device, name);
//	printf ("VAL=");
 
//		printf("****************************************************");
	while (1){
		if ((rd = read (fd, ev, size * 64)) < size){
			perror ("Read error");
			return 1;
		}

		for (i=0; i< ((int)(rd/size)); i++) {
			if (ev[i].type == EV_KEY) {
				if ((ev[i].value == KEY_PRESS) || (ev[i].value == KEY_KEEPING_PRESSED)) {
					if (ev[i].code < 12) {
						printf ("%d", (decode_scancode[ev[i].code]));
					}
					else if (ev[i].code == 28) {
						printf ("\n");
						return 0;
					}
				}
			}
		}
  }
//		printf("****************************************************");
 
  return 0;
}
