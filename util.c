/*
   Copyright (C) Andrew Tridgell 2002
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "ccache.h"

static FILE *logfile;

/* log a message to the CCACHE_LOGFILE location */
void cc_log(const char *format, ...)
{
	int ret;
	va_list ap;
	extern char *cache_logfile;

	if (!cache_logfile) return;

	if (!logfile) logfile = fopen(cache_logfile, "a");
	if (!logfile) return;
	
	va_start(ap, format);
	ret = vfprintf(logfile, format, ap);
	va_end(ap);
	fflush(logfile);
}

/* something went badly wrong! */
void fatal(const char *msg)
{
	cc_log("FATAL: %s\n", msg);
	exit(1);
}

/* copy all data from one file descriptor to another */
void copy_fd(int fd_in, int fd_out)
{
	char buf[1024];
	int n;

	while ((n = read(fd_in, buf, sizeof(buf))) > 0) {
		if (write(fd_out, buf, n) != n) {
			fatal("Failed to copy fd");
		}
	}
}


/* make sure a directory exists */
int create_dir(const char *dir)
{
	struct stat st;
	if (stat(dir, &st) == 0) {
		if (S_ISDIR(st.st_mode)) {
			return 0;
		}
		errno = ENOTDIR;
		return 1;
	}
	if (mkdir(dir, 0777) != 0) {
		return 1;
	}
	return 0;
}

/*
  this is like asprintf() but dies if the malloc fails
*/
int x_asprintf(char **ptr, const char *format, ...)
{
	va_list ap;
	int ret;
	
	*ptr = NULL;
	va_start(ap, format);
	ret = vsnprintf(NULL, 0, format, ap);
	va_end(ap);

	if (ret <= 0) {
		fatal("Bad vsnprintf implementation!?\n");
	}

	*ptr = (char *)malloc(ret+1);
	if (! *ptr) {
		fatal("out of memory in x_asprintf\n");
	}
	va_start(ap, format);
	ret = vsnprintf(*ptr, ret+1, format, ap);
	va_end(ap);

	return ret;
}

/*
  this is like strdup() but dies if the malloc fails
*/
char *x_strdup(const char *s)
{
	char *ret;
	ret = strdup(s);
	if (!ret) {
		fatal("out of memory in strdup\n");
	}
	return ret;
}
