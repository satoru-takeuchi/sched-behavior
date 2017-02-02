#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err.h>

#define NLOOP_FOR_ESTIMATION 10000000000L

static inline long diff_usec(struct timeval before, struct timeval after)
{
	return (after.tv_sec * 1000000L + after.tv_usec)
		- (before.tv_sec * 1000000L + before.tv_usec);
}

static double loops_per_usec()
{
	long i;
	struct timeval before, after;

	if (gettimeofday(&before, NULL) == -1)
		err(EXIT_FAILURE, "gettimeofday(before) failed");

	for (i = 0; i < NLOOP_FOR_ESTIMATION; i++)
		;

	if (gettimeofday(&after, NULL) == -1)
		err(EXIT_FAILURE, "gettimeofday(after) failed");

	return NLOOP_FOR_ESTIMATION / diff_usec(before, after);
}

static inline void load(int nloop)
{
	int i;
	for (i = 0; i < nloop; i++)
		;
}

static void child_fn(pid_t id, struct timeval *buf, int nrecord, int nloop_per_resol, struct timeval start)
	__attribute__ ((noreturn));
static void child_fn(pid_t id, struct timeval *buf, int nrecord, int nloop_per_resol, struct timeval start)
{
	int i;
	for (i = 0; i < nrecord; i++) {
		load(nloop_per_resol);

		struct timeval tv;
		if (gettimeofday(&tv, NULL) == -1)
			err(EXIT_FAILURE, "gettimeofday() failed");
		buf[i] = tv;
	}
	for (i = 0; i < nrecord; i++) {
		printf("%d,%ld,%f\n", id, diff_usec(start, buf[i]), (double)(i+1)/nrecord);
	}
	exit(EXIT_SUCCESS);
}

static void parent_fn(int nproc)
{
	int i;
	for (i = 0; i < nproc; i++)
		wait(NULL);
}

static struct timeval **logbuf;
static pid_t *pids;

int main(int argc, char *argv[])
{
	int ret = EXIT_FAILURE;

	if (argc < 4) {
		fprintf(stderr, "usage: %s <nproc> <total[us]> <resolution[us]>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int nproc = atoi(argv[1]);
	int total = atoi(argv[2]);
	int resol = atoi(argv[3]);

	if (nproc < 1) {
		fprintf(stderr, "<nproc>(%d) should be >= 1\n", nproc);
		exit(EXIT_FAILURE);
	}

	if (total < 1) {
		fprintf(stderr, "<total>(%d) should be >= 1\n", total);
		exit(EXIT_FAILURE);
	}

	if (resol < 1) {
		fprintf(stderr, "<resol>(%d) should be >= 1\n", resol);
		exit(EXIT_FAILURE);
	}

	if (total % resol) {
		fprintf(stderr, "<total>(%d) should be multiple of <resolution>(%d)\n", total, resol);
		exit(EXIT_FAILURE);
	}
	int nrecord = total / resol;
	long pagesize = sysconf(_SC_PAGESIZE);
	if (pagesize < 0)
		err(EXIT_FAILURE, "sysconf() failed");

	logbuf = malloc(nproc * sizeof(struct timeval *));
	if (logbuf == NULL)
		err(EXIT_FAILURE, "malloc(logbuf) failed");

	int i, nallocated;
	for (i = 0, nallocated = 0; i < nproc; i++) {
		int ret_pma = posix_memalign((void **)&logbuf[i], pagesize, nrecord * sizeof(struct timeval));
		if (ret_pma) {
			errno = ret;
			warn("posix_memalign() failed");
			goto free_logbuf;
		}
		nallocated++;
		if (mlock(logbuf[i], nrecord * sizeof(struct timeval))) {
			warn("mlock() failed");
			goto free_logbuf;
		}
	}

	int nloop_per_resol = (int)(loops_per_usec() * resol);

	pids = malloc(nproc * sizeof(pid_t));
	if (pids == NULL) {
		warn("malloc(pids) failed");
		goto free_logbuf;
	}

	struct timeval start;
	if (gettimeofday(&start, NULL) == -1) {
		warn("gettimeofday(start) failed");
		goto free_logbuf;
	}
	int ncreated;
	for (i = 0, ncreated = 0; i < nproc; i++, ncreated++) {
		pids[i] = fork();
		if (pids[i] < 0) {
			goto wait_children;
		} else if (pids[i] == 0) {
			// children

			child_fn(i, logbuf[i], nrecord, nloop_per_resol, start);
			/* shouldn't reach here */
		}
	}
	ret = EXIT_SUCCESS;

	// parent

wait_children:
	if (ret == EXIT_FAILURE)
		for (i = 0; i < ncreated; i++)
			if (kill(pids[i], SIGINT) < 0)
				warn("kill() failed");

	for (i = 0; i < ncreated; i++)
		if (wait(NULL) < 0)
			warn("wait() failed.");

free_pids:
	free(pids);

free_logbuf:
	for (i = 0; i < nallocated; i++)
		free(logbuf[i]);
	free(logbuf);

	exit(ret);
}
