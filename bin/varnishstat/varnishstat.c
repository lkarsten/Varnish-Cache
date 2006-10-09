/*-
 * Copyright (c) 2006 Verdens Gang AS
 * Copyright (c) 2006 Linpro AS
 * All rights reserved.
 *
 * Author: Poul-Henning Kamp <phk@phk.freebsd.dk>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id$
 *
 * Log tailer for Varnish
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <time.h>

#ifndef HAVE_CLOCK_GETTIME
#include "compat/clock_gettime.h"
#endif

#include "libvarnish.h"
#include "shmlog.h"
#include "varnishapi.h"

static void
myexp(double *acc, double val, unsigned *n, unsigned nmax)
{

	if (*n < nmax)
		(*n)++;
	(*acc) += (val - *acc) / (double)*n;
}

static void
do_curses(struct varnish_stats *VSL_stats, int delay)
{
	struct varnish_stats copy;
	intmax_t ju;
	struct timespec ts;
	double tt, lt, hit, miss, ratio, up;
	double a1, a2, a3;
	unsigned n1, n2, n3;
	time_t rt;
	int i;


	memset(&copy, 0, sizeof copy);

	a1 = a2 = a3 = 0.0;
	n1 = n2 = n3 = 0;

	initscr();
	erase();

	lt = 0;
	while (1) {
		clock_gettime(CLOCK_REALTIME, &ts);
		tt = ts.tv_nsec * 1e-9 + ts.tv_sec;
		lt = tt - lt;

		rt = ts.tv_sec - VSL_stats->start_time;
		up = rt;

		move(0,0);
		i = 0;
		if (rt > 86400) {
			printw("%dd+", rt / 86400);
			rt %= 86400;
			i++;
		}
		printw("%02d:", rt / 3600);
		rt %= 3600;
		printw("%02d:", rt / 60);
		rt %= 60;
		printw("%02d\n", rt);
		hit = (intmax_t)VSL_stats->cache_hit -
		    (intmax_t)copy.cache_hit;
		miss = (intmax_t)VSL_stats->cache_miss -
		    (intmax_t)copy.cache_miss;
		hit /= lt;
		miss /= lt;
		if (hit + miss != 0) {
			ratio = hit / (hit + miss);
			myexp(&a1, ratio, &n1, 10);
			myexp(&a2, ratio, &n2, 100);
			myexp(&a3, ratio, &n3, 1000);
		}
		printw("Hitrate ratio: %8u %8u %8u\n", n1, n2, n3);
		printw("Hitrate avg:   %8.4f %8.4f %8.4f\n", a1, a2, a3);
		printw("\n");

#define MAC_STAT(n,t,f,d) \
		ju = VSL_stats->n; \
		printw("%12ju %12.2f %12.2f " d "\n", \
		    ju, (ju - (intmax_t)copy.n)/lt, ju / up); \
		copy.n = ju;
#include "stat_field.h"
#undef MAC_STAT
		lt = tt;
		refresh();
		sleep(delay);
	}
}

static void
usage(void)
{
	fprintf(stderr, "usage: varnishstat [-1V] [-w delay]\n");
	exit(1);
}

int
main(int argc, char **argv)
{
	int c;
	struct varnish_stats *VSL_stats;
	int delay = 1, once = 0;

	VSL_stats = VSL_OpenStats();

	while ((c = getopt(argc, argv, "1Vw:")) != -1) {
		switch (c) {
		case '1':
			once = 1;
			break;
		case 'V':
			varnish_version("varnishstat");
			exit(0);
		case 'w':
			delay = atoi(optarg);
			break;
		default:
			usage();
		}
	}

	if (!once) {
		do_curses(VSL_stats, delay);
	} else {

#define MAC_STAT(n,t,f,d) \
		printf("%12ju  " d "\n", (VSL_stats->n));
#include "stat_field.h"
#undef MAC_STAT
	}

	exit (0);

}
