/* cont -- program to execute continously a program.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Tue Jul  9 08:25:53 EEST 2019
 * Copyright: (C) 2019 LUIS COLORADO.  All rights reserved.
 * License: BSD
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define F(_fmt) "%s:%d: " _fmt, __FILE__, __LINE__

#ifndef VERSION
#error set VERSION in makefile
#endif

#define FLAG_VERBOSE	(1<<0)
#define FLAG_VERSION	(1<<1)
#define FLAG_DELAY		(1<<2)
#define FLAG_NTIMES		(1<<3)
#define FLAG_STOP		(1<<4)
#define FLAG_NOESCAPES	(1<<5)
#define FLAG_REDSTDERR	(1<<6)

int flags = 0;
useconds_t delay = 1000000;
size_t ntimes;

volatile int cont = 1;

void handler()
{
	cont = 0;
}

void doVersion(void)
{
	fprintf(stderr, 
		"cont: " VERSION "\n"
		"  (C) 2019 Luis Colorado.  All rights reserved.\n"
		"  License: BSD\n"
		"Usage: cont [ -Vev ] [ -n num ] [ -t timespec ] command [ args ... ]\n"
		"Where:\n"
		"  -V indicates to show the version string for the program.  It shows\n"
		"     this screen.\n"
		"  -e disables the output of escape sequences.  The program limits to\n"
		"     execute and pass the whole output to stdout.  No interspersion\n"
		"     of escapes is done.\n"
		"  -v Be verbose.  The program output on stderr the command about to \n"
		"     be executed, and once it end (after all executions) shows the\n"
		"     total number of lines and executions that has made.\n"
		"  -n makes only `num' executions and ends after all.  If not\n"
		"     specified, the program runs until it is interrupted by a signal.\n"
		"  -t Allows to set a different time between runs.  timespec is\n"
		"     specified as a decimal number of seconds (dot and decimals is\n"
		"     allowed) with a usec resolution.\n"
		"  -r Allows to catch also the stderr of the command, so in case it\n"
		"     generates errors, it is also overwritten by the next execution\n");
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
	} else if (res == 0) { /* CHILD PROCESS */
		close(fd[0]); /* not going to use it */
		dup2( fd[1], 1); /* redirect output to pipe */
		if (flags & FLAG_REDSTDERR)
			dup2( fd[1], 2); /* redirect stderr of child */
		close(fd[1]);

		execvp(argv[0], argv);

		fprintf(stderr,
			F("execv: %s: ERROR %d: %s\n"),
			argv[0], errno, strerror(errno));
		exit(EXIT_FAILURE);
	} else { /* PARENT PROCESS */
		pid_t cld_pid = res;
		close(fd[1]); /* no writing to the pipe */
		FILE *f = fdopen(fd[0], "rt"); /* just reading */
		int c;
		size_t lines = 0; /* need to count lines */
		while((c = fgetc(f)) != EOF) {
			if (c == '\n') {
				lines++;
				fflush(stdout);
				if (!(flags & FLAG_NOESCAPES)) {
					/* DELETE TO END OF LINE */
					fputs("\033[K", stderr);
					fflush(stderr);
				}
			}
			putc(c, stdout);
		}
		if (!(flags & FLAG_NOESCAPES)) {
			/* DELETE TO END OF SCREEN */
			fputs("\033[J", stderr);
			fflush(stderr);
		}
		int status;
		wait(&status);
		fclose(f); /* should close fd[0] */
		close(fd[0]);
		if (   WIFEXITED(status)
			&& WEXITSTATUS(status) == EXIT_SUCCESS) {
			return lines;
		} else {
			fprintf(stderr,
				F("child process exited with code = %d\n"),
				WEXITSTATUS(status));
			return -1;
		}
	}
} /* loop */

int main(int argc, char **argv)
{
	int opt;
	float t;

	while ((opt = getopt(argc, argv, "Venr:t:v")) >= 0) {
		switch(opt) {
		case 'V': flags |= FLAG_VERSION;
				  break;
		case 'e': flags |= FLAG_NOESCAPES;
				  break;
		case 'n': flags |= FLAG_NTIMES;
				  ntimes = atoi(optarg);
				  break;
		case 't': flags |= FLAG_DELAY;
				  t = atof(optarg);
				  break;
		case 'v': flags |= FLAG_VERBOSE;
				  break;
		case 'r': flags |= FLAG_REDSTDERR;
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

	if (!argc) {
		fprintf(stderr, F("ERROR: no command specified\n"));
		exit(EXIT_FAILURE);
	}

	if (flags & FLAG_VERBOSE) {
		char *sep = "About to execute: ";
		int i;
		fprintf(stderr, F(""));
		for (i = 0; i < argc; i++) {
			fprintf(stderr, "%s%s", sep, argv[i]);
			sep = " ";
		}
		fprintf(stderr, "\n");
	}

	if (flags & FLAG_DELAY) {
		delay = t * 1.0E6;
	}

	signal(SIGINT, handler);
	signal(SIGHUP, handler);

	size_t total_lines = 0;
	size_t total_execs = 0;
	ssize_t n = 0;
	while(cont && (!(flags & FLAG_NTIMES) || ntimes--)) {

		/* move up as many lines as input from subcommand */
		if (n && !(flags & FLAG_NOESCAPES)) {
			fprintf(stderr, "\r\033[%uA", (unsigned) n);
			fflush(stderr);
		}

		n = loop(argc, argv);

		if (n < 0) {
			/* we have already written the error */
			exit(EXIT_FAILURE);
		}

		usleep(delay);

		total_lines += n;
		total_execs++;
	}
	if (flags & FLAG_VERBOSE) {
		fprintf(stderr,
			F("Total lines: %lu\n"),
			(unsigned long) total_lines); 
		fprintf(stderr,
			F("Total execs: %lu\n"),
			(unsigned long) total_execs);
	}
	exit(EXIT_SUCCESS);
} /* main */
