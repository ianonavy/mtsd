/*
 * The MIT License (MIT)
 * Copyright (c) 2014 Ian Naval
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "helpers.h"

void fatal(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void daemonize(void)
{
	pid_t pid = fork();
	if (pid < 0) {
		exit(1);
	} else if (pid != 0) {
		exit(EXIT_SUCCESS);
	} else {
		// signal(SIGCHLD, SIG_IGN);
		// signal(SIGHUP, SIG_IGN);

		/* Change the file mode mask */
		umask(0);

		/* Change the current working directory */
		if ((chdir("/")) < 0) {
			/* Log any failures here */
			exit(EXIT_FAILURE);
		}

		pid = fork();
		if (pid < 0) {
			exit(1);
		} else if (pid != 0) {
			PRINTF("Process ID is %d\n", pid);
			exit(EXIT_SUCCESS);
		}
	}

}
