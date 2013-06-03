/*
 * Copyright (C) 2011 Dwi Sasongko S <ruckuus@gmail.com>
 * Author: Dwi Sasongko S <ruckuus@gmail.com>
 *
 * This file is a small program to operate CD-510
 * with DSP-800 mode.
 *
 * [LICENSE]
 * [/LICENSE]
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

#define BAUDRATE		B9600
#define ACTION_INIT		14
#define ACTION_SELFTEST		1
#define ACTION_SELECT_CHARSET	2
#define	ACTION_MOVE_CURSOR	3
#define ACTION_CLEAR		4
#define ACTION_SAVE		5
#define	ACTION_DISPLAY		6
#define ACTION_SET_BRIGHTNSS	7
#define ACTION_BLINK		8
#define ACTION_DNL_CHAR		9
#define ACTION_DEL_CHAR		10
#define ACTION_SELECT		11
#define ACTION_WRITE		12
#define ACTION_UNKNOWN		13

#define zalloc(size)	calloc(size, 1)

#define dbg(format, arg...)						\
	do {								\
		if (debug)						\
			fprintf(stdout, "cdisplay: %s: " format, __func__ ,	\
			## arg); 					\
	} while (0)


static int debug;
static int verbose;

struct session {
	char *configfile;
	char *charset;
	char *device;
	char *words;
	char *command;
	char *time;
	char *homedir;
	int action;
	int fd;
	int overwrite;
};

static void lcd_init(int fd)
{
	dbg("Initialize Display\n");
	char cmd[2] = {0x1B, 0x40};
	write(fd, cmd, 2);

}

static void lcd_selftest(int fd)
{
	dbg("Running Self Test\n");
	char cmd[4] = {0x04, 0x01, 0x40, 0x17};
	write(fd, cmd, 4);
}

static void lcd_clear(int fd)
{
	dbg("clear LCD\n");
	char cmd[6] = {0x04, 0x01, 0x43, 0x31, 0x58, 0x17};
	write(fd, cmd, 6);
}

static void lcd_set_brightness(int fd, int level)
{
}

static void lcd_blink(int fd)
{
}

static void lcd_display(int fd)
{}

static void display_help()
{
	fprintf(stdout, "cdisplay usage\n");
	fprintf(stdout, "--action\n");
	fprintf(stdout, "(init, clear, blink, selftest)\n");
	fprintf(stdout, "--device: specify the device\n");
	fprintf(stdout, "--words: specify words\n");
	fprintf(stdout, "--overwrite: overwrite display\n");
	fprintf(stdout, "--charset\n");
	fprintf(stdout, "--debug\n");
	fprintf(stdout, "--verbose\n");


}

struct session *session_alloc(void)
{
	struct session *session;
	session = zalloc(sizeof(*session));
	if (!session)
		return NULL;
	return session;

}

void send_char(int fd, char *str)
{
	int len, rv;
	
	if (fd <= 0) {
		perror("Device not recognized!: ");
	}

	if (str == NULL) {
		perror("String is empty!: ");
	}

	len = strlen(str);
	if (write(fd, str, len) < len) {
		perror("Error writing!: ");
	}
}

static int send_command(struct session *session)
{
	struct termios new, old;
	char *device;
	char *words;
	char *charset;
	int action;
	int fd;
	int rv;

	if (!session->device) {
		dbg("Error, you must specify the device\n");
		display_help();
		goto err0;
	}

	if ((!session->words) && (!session->action)) {
		dbg("Error, you must specify words\n");
		display_help();
		goto err0;
	}

	device = session->device;
	charset = session->charset;
	action = session->action;

	fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd < 0) {
		perror(device);
	}

	/* save current setting */
	tcgetattr(fd, &old);
	bzero(&new, sizeof(new));

	/* setting baud rate */
	cfsetispeed(&new, BAUDRATE);
	cfsetospeed(&new, BAUDRATE);

	/* setting parity */
	new.c_cflag &= ~PARENB;
	new.c_cflag &= ~CSTOPB;
	new.c_cflag &= ~CSIZE;
	new.c_cflag |= CS8;

	/* setting hardware flow control */
	new.c_cflag |= CRTSCTS;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &new);

	if (session->words) {
		if (session->overwrite)
			lcd_clear(fd);
		send_char(fd, session->words);
	}

	switch (action) {
		case ACTION_INIT:
			lcd_init(fd);
			break;
		case ACTION_SELFTEST:
			lcd_selftest(fd);
			break;
		case ACTION_CLEAR:
			lcd_clear(fd);
			break;
		case ACTION_SET_BRIGHTNSS:
			lcd_set_brightness(fd, 10);
			break;
		case ACTION_BLINK:
			lcd_blink(fd);
			break;
		case ACTION_DISPLAY:
			lcd_display(fd);
			break;
	//	default:
	//		display_help();
	}

	return 0;

err0:
	free(session->device);
	free(session->words);

}
static void parse_configfile(struct session *session)
{
	FILE *config_file;
	char *line = NULL;
	size_t len = 0;
	char *charset;
	char *device;
	char *words;

	config_file = fopen(session->configfile, "r");
	
	if (config_file == NULL)
		return;

	do {
		ssize_t n = getline(&line, &len, config_file);
		if (n < 0)
			break;
		if (line[n-1] == '\n')
			line[n-1] = '\0';

		//*strchrnul(line, '#') = '\0';
		char *c = line;
		while (isspace(*c))
			c++;
		/* Ignore blank lines */
		if (c[0] == '\0')
		       continue;

		if (!strncasecmp(c, "charset", 7) && (c[7] == '=')) {
			c += 8;
			if (c[0] != '\0')
				charset = strdup(c);
		} else if (!strncasecmp(c, "device", 6) && (c[6] == '=')) {
			c += 7;
			if (c[0] != '\0')
				device = strdup(c);
		} else if (!strncasecmp(c, "words", 5) && (c[5] == '=' )) {
			c+= 6;
			if (c[0] != '\0')
				words = strdup(c);
		} 

	} while (!feof(config_file));

	if (charset)
		session->charset = charset;
	if (device)
		session->device = device;
	if (words)
		session->words = words;

	free(line);
	fclose(config_file);
}

int main(int argc, char *argv[], char *envp[])
{
	static const struct option options[] = {
		{ "debug", 0, NULL, 'd' },
		{ "verbose", 0, NULL, 'V'},
		{ "charset", 1, NULL, 'C'},
		{ "device", 1, NULL, 'D' },
		{ "words", 1, NULL, 'w' },
		{ "action", 1, NULL, 'a' },
		{ "overwrite", 0, NULL, 'o'},
		{ }
	};

	struct session *session;
	pid_t child;
	time_t t;
	int option;

	debug = 0;
	verbose = 0;

	session = session_alloc();
	if (!session) {
		fprintf(stderr, "no more memory ...\n");
		return -1;
	}

	time(&t);
	session->time = strdup(ctime(&t));
	session->time[strlen(session->time)-1] = 0x00;

	session->homedir = strdup(getenv("HOME"));

	session->configfile = zalloc(strlen(session->homedir) + 7);
	sprintf(session->configfile, "%s/.config", session->homedir);

	parse_configfile(session);

	while (1) {
		option = getopt_long_only(argc, argv, "dVC:D:w:a:o", options, NULL);
		if (option == -1)
			break;
		switch (option) {
			case 'd':
				debug = 1;
				break;
			case 'V':
				verbose = 1;
				break;
			case 'C':
				if (session->charset)
					free(session->charset);

				session->charset = strdup(optarg);
				break;
			case 'D':
				if (session->device)
					free(session->device);

				session->device = strdup(optarg);
				break;
			case 'w':
				session->words = strdup(optarg);
				break;
			case 'o':
				session->overwrite = 1;
				break;
			case 'a':
				if (strcasecmp(optarg, "init") == 0)
					session->action = ACTION_INIT;
				else if (strcasecmp(optarg, "selftest") == 0)
					session->action = ACTION_SELFTEST;
				else if (strcasecmp(optarg, "clear") == 0)
					session->action = ACTION_CLEAR;
				else if (strcasecmp(optarg, "save") == 0)
					session->action = ACTION_SAVE;
				else if (strcasecmp(optarg, "display") == 0)
					session->action = ACTION_DISPLAY;
				else if (strcasecmp(optarg, "setbright") == 0)
					session->action = ACTION_SET_BRIGHTNSS;
				else if (strcasecmp(optarg, "blink") == 0)
					session->action = ACTION_BLINK;
				else 
					session->action = ACTION_UNKNOWN;
				break;
			default:
				display_help();
		}
	}

	/* DEBUG */
#if 0
	if (debug)
		printf ("debug\n");
	if (verbose)
		printf ("verbose\n");

	if (session->action)
		printf("%d\n", session->action);

	if (session->words)
		printf("%s\n", session->words);

	if (session->device)
		printf("%s\n", session->device);

	if (session->charset)
		printf("%s\n", session->charset);

#endif
	/* END DEBUG */

	int rv = 0;
	rv = send_command(session);

	if (rv < 0)
		fprintf(stderr, "Error sending command");

}

