/*
 * Copyright (c) 2019 Sebastien Marie <semarie@online.fr>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * TEST:
 * - create a pipe
 * - start N readers and M writers
 * - writers will write random chars (and keep track of them)
 * - readers will read them in (and keep track of them)
 * - at timeout, writers quit, and the pipe is closed
 * - at eof, readers quit
 * - program checks if all chars written have been read
 */

#include <sys/pipe.h>

#include <err.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void dosig(int);
void *reader(void *);
void *writer(void *);

volatile int quit = 0;

struct arg {
	int id;
	int fd;
	int bufsize;
	int count_chars[256];
};

void
dosig(int sig)
{
	if (sig != SIGALRM)
		return;

	dprintf(STDIN_FILENO, "  timeout finished, asking writers to quit\n");
	quit = 1;
}

void *
reader(void *arg)
{
	struct arg *ra = (struct arg *)arg;
	u_char *buf;
	ssize_t n = 1;
	int i;

	if ((buf = malloc(ra->bufsize)) == NULL)
		err(EXIT_FAILURE, "reader %d: malloc", ra->id);

	while (n != 0) {
		/* read data */
		if ((n = read(ra->fd, buf, ra->bufsize)) == -1)
			err(EXIT_FAILURE, "reader %d: read", ra->id);

		/* count chars readed */
		for (i = 0; i < n; i++)
			ra->count_chars[(int)buf[i]]++;
	}

	free(buf);

	return arg;
}

void *
writer(void *arg)
{
	struct arg *wa = (struct arg *)arg;
	u_char *buf;
	ssize_t n;
	int i;

	if ((buf = malloc(wa->bufsize)) == NULL)
		err(EXIT_FAILURE, "writer %d: malloc", wa->id);

	while (quit == 0) {
		/* fill buf */
		arc4random_buf(buf, wa->bufsize);

		/* write it */
		if ((n = write(wa->fd, buf, wa->bufsize)) == -1)
			err(EXIT_FAILURE, "writer %d: write", wa->id);

		/* keep track of what was written */
		for (i=0; i < n; i++)
			wa->count_chars[(int)buf[i]]++;
	}

	free(buf);

	return arg;
}

int
main(int argc, char *argv[])
{
	pthread_t *th;
	const char *errstr;
	int timeout = 5;
	int nreaders;
	int nwriters;
	int ch, nth, i, j;
	int pfds[2];
	int count_chars[256];
	int count = 0;

	if (pledge("stdio", "") == -1)
		err(EXIT_FAILURE, "pledge");

	/* random values by default */
	nreaders = 1 + arc4random_uniform(10);
	nwriters = 1 + arc4random_uniform(10);

	/* parse args */
	while ((ch = getopt(argc, argv, "t:r:w:")) != -1) {
		switch (ch) {
		case 't':
			timeout = strtonum(optarg, 1, 60, &errstr);
			if (errstr != NULL)
				errx(EXIT_FAILURE, "timeout is %s: %s",
				    errstr, optarg);
			break;
		case 'r':
			nreaders = strtonum(optarg, 1, 256, &errstr);
			if (errstr != NULL)
				errx(EXIT_FAILURE, "nreaders is %s: %s",
				    errstr, optarg);
			break;
		case 'w':
			nwriters = strtonum(optarg, 1, 256, &errstr);
			if (errstr != NULL)
				errx(EXIT_FAILURE, "nwriters is %s: %s",
				    errstr, optarg);
			break;
		default:
			errx(EXIT_FAILURE, "usage");
		}
	}
	argc -= optind;
	argv += optind;

	/* print out informations */
	printf("- timeout: %ds\n  readers: %d\n  writers: %d\n",
	    timeout, nreaders, nwriters);

	/* create a pipe */
	if (pipe(pfds) == -1)
		err(EXIT_FAILURE, "pipe");
	
	/* create array for threads */
	nth = nreaders + nwriters;
	if ((th = reallocarray(NULL, sizeof(pthread_t), nth)) == NULL)
		err(EXIT_FAILURE, "reallocarray");

	/* create threads */
	printf("- starting readers and writers\n");
	for (i = 0; i < nth; i++) {
		struct arg *a = calloc(sizeof(*a), 1);
		a->id = i;
		a->bufsize = arc4random_uniform(PIPE_SIZE*2);

		if (i < nreaders) {
			a->fd = pfds[0];

			if (pthread_create(&th[i], NULL, reader, a) != 0)
				err(EXIT_FAILURE, "pthread_create: reader %d", a->id);

			printf("th[%d]: reader (bufsize %d)\n", a->id, a->bufsize);
		} else {
			a->fd = pfds[1];

			if (pthread_create(&th[i], NULL, writer, a) != 0)
				err(EXIT_FAILURE, "pthread_create: writer %d", a->id);

			printf("th[%d]: writer (bufsize %d)\n", a->id, a->bufsize);
		}
	}

	/* clear count_chars */
	for (i = 0; i < 256; i++)
		count_chars[i] = 0;

	/* install an alarm to quit */
	signal(SIGALRM, dosig);
	alarm(timeout);

	/* join writers */
	printf("- waiting for writers (%ds)\n", timeout);
	for (i = nreaders; i < nth; i++) {
		struct arg *a;

		if (pthread_join(th[i], (void **)&a) != 0)
			err(EXIT_FAILURE, "pthread_join: writer %d", a->id);

		for (j = 0; j < 256; j++) {
			count_chars[j] += a->count_chars[j];
			count += a->count_chars[j];
		}

		free(a);
	}

	/* close pipe write side */
	close(pfds[1]);

	/* join readers */
	printf("- waiting for readers\n");
	for (i = 0; i < nreaders; i++) {
		struct arg *a;

		if (pthread_join(th[i], (void **)&a) != 0)
			err(EXIT_FAILURE, "pthread_join: reader %d", a->id);

		for (j = 0; j < 256; j++)
			count_chars[j] -= a->count_chars[j];

		free(a);
	}

	/* close pipe read side */
	close(pfds[0]);

	printf("- checking results\n");
	
	/* check count_chars (same number of reads and writes for each char) */
	for (i = 0; i < 256; i++)
		if (count_chars[i] != 0)
			errx(EXIT_FAILURE, "count_chars[%d] == %d (!= 0)",
			    i, count_chars[i]);

	/* check count (something was written) */
	if (count == 0)
		errx(EXIT_FAILURE, "nothing written");

	return EXIT_SUCCESS;
}
