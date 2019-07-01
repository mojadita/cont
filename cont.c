#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define F(_fmt) "%s:%d: " _fmt, __FILE__, __LINE__

#define FLAG_VERBOSE	(1<<0)
#define FLAG_VERSION	(1<<1)
#define FLAG_DELAY		(1<<2)
#define FLAG_NTIMES		(1<<3)

int flags = 0;
useconds_t delay = 1000000;
size_t ntimes;

void doVersion(void)
{
	fprintf(stderr, 
		"cont: v1.0\n"
		"(C) Luis Colorado.  All rights reserved.\n"
		"License: BSD\n");
	exit(EXIT_SUCCESS);
}

ssize_t loop(int argc_unused, char **argv)
{
	int fd[2];
	int res = pipe(fd);

	res = fork();
	if (res < 0) {
		fprintf(stderr,
			F("fork: ERROR %d: %s\n"),
			errno,
			strerror(errno));
		return -1;
	} else if (res == 0) { /* child */
		close(fd[0]); /* not going to use it */
		dup2(fd[1], 1); /* redirect output to pipe */
		close(fd[1]);

		execvp(argv[0], argv);

		fprintf(stderr,
			F("execv: ERROR %d: %s\n"),
			errno, strerror(errno));
		return -1;
	} else { /* parent */
		pid_t cld_pid = res;
		close(fd[1]); /* no writing to the pipe */
		FILE *f = fdopen(fd[0], "rt"); /* just reading */
		int c;
		size_t lines = 0;
		while((c = fgetc(f)) != EOF) {
			if (c == '\n') {
				lines++;
				fflush(stdout);
				fputs("\033[K", stderr);
				fflush(stderr);
			}
			putc(c, stdout);
		}
		fputs("\033[J", stderr);
		wait(NULL);
		return lines;
	}

} /* loop */

int main(int argc, char **argv)
{
	int opt;
	float t;

	while ((opt = getopt(argc, argv, "t:Vvn:")) >= 0) {
		switch(opt) {
		case 't': flags |= FLAG_DELAY;
				  t = atof(optarg);
				  break;
		case 'V': flags |= FLAG_VERSION;
				  break;
		case 'v': flags |= FLAG_VERBOSE;
				  break;
		case 'n': flags |= FLAG_NTIMES;
				  ntimes = atoi(optarg);
				  break;
		/* ... */
		}
	}

	if (flags & FLAG_VERSION)
		doVersion();

	/* the next pair of sentences is like `shift optind' in the shell. */
	/* trick, don't move the parameters, just move the pointer */
	argc -= optind; /* adjust the number of parameters. */
	argv += optind; /* advance the pointer to the proper place */

	if (flags & FLAG_VERBOSE) {
		char *sep = "About to execute:";
		int i;
		for (i = 0; i < argc; i++) {
			fprintf(stderr, "%s%s", sep, argv[i]);
			sep = " ";
		}
		fprintf(stderr, "\n");
	}

	if (flags & FLAG_DELAY) {
		delay = t * 1.0E6;
	}

	size_t total_lines = 0;
	ssize_t n = 0;
	while(!(flags & FLAG_NTIMES) || ntimes--) {

		/* move up as many lines as input from subcommand */
		if (n) fprintf(stderr, "\r\033[%ldA", n);

		n = loop(argc, argv);

		if (n < 0) {
			/* we have already written the error */
			exit(EXIT_FAILURE);
		}

		usleep(delay);

		total_lines += n;
	}
	if (flags & FLAG_VERBOSE) {
		fprintf(stderr,
			F("Total lines: %lu\n"),
			total_lines);
	}
	exit(EXIT_SUCCESS);
}
