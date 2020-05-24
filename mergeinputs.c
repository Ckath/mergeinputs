#include <errno.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int fdo;
pthread_t *threads;

static void
emit(int type, int code, int val)
{
   struct input_event ie;

   ie.type = type;
   ie.code = code;
   ie.value = val;
   /* timestamp values below are ignored */
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;

   write(fdo, &ie, sizeof(ie));
}

static void
input_redirect(char *path)
{
	int fdi = open(path, O_RDONLY);
	if (fdi == -1) {
		fprintf(stderr, "failed to open input from %s: %s\n", 
				path, strerror(errno));
	} if (ioctl(fdi, EVIOCGRAB, 1) == -1) {
		fprintf(stderr, "failed to grab input %s: %s\n", 
				path, strerror(errno));
	}
	struct input_event ev;
	while (read(fdi, &ev, sizeof(ev))) {
		emit(ev.type, ev.code, ev.value);
	}
	fprintf(stderr, "lost input %s: %s\n",
			path, strerror(errno));
}

int
main(int argc, char *argv[])
{
	if (argc < 1 || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
		fputs("mergeinputs inputeventpaths\n", stderr);
		exit(1);
	}

	fdo = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fdo == -1) {
		fprintf(stderr, "failed to open uinput: %s\n", strerror(errno));
		exit(1);
	}

	/* claim bits for virtual device */
	if (ioctl(fdo, UI_SET_EVBIT, EV_KEY) == -1) {
		fprintf(stderr, "failed to set evbits: %s\n", strerror(errno));
	} if (ioctl(fdo, UI_SET_EVBIT, EV_SYN) == -1) {
		fprintf(stderr, "failed to set evbits: %s\n", strerror(errno));
	} if (ioctl(fdo, UI_SET_EVBIT, EV_MSC) == -1) {
		fprintf(stderr, "failed to set evbits: %s\n", strerror(errno));
	}
	for (unsigned k = 0; k < KEY_MAX; ++k) {
		if (ioctl(fdo, UI_SET_KEYBIT, k) == -1) {
			fprintf(stderr, "failed to set keybits: %s\n", strerror(errno));
			exit(1);
		}
	}
   
	/* register virtual 'merged' device in uinput */
	struct uinput_setup usetup;
	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = 0xC81D;
	usetup.id.product = 0xC800;
	strcpy(usetup.name, "merged input");
	ioctl(fdo, UI_DEV_SETUP, &usetup);
	ioctl(fdo, UI_DEV_CREATE);
	sleep(1); /* give kernel a chance to recognize it */

	/* create a redirect thread for each input */
	for (int a = 1; a < argc; ++a) {
		threads = realloc(threads, sizeof(pthread_t)*a);
		pthread_create(&threads[a-1], NULL, (void *)input_redirect, argv[a]);
	}

	/* sleep main thread until interrupted */
	pause();
	fprintf(stderr, "stopped running with reason: %s\n", strerror(errno));
	ioctl(fdo, UI_DEV_DESTROY);
	close(fdo);
	free(threads);
}
