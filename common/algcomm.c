/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024, 2025 Jianping Duan <static.integer@hotmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#include "algcomm.h"
#include <stdarg.h>

/* Prints error message and exit. */
void
errmsg_exit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	exit(EXIT_FAILURE);
}

/* 
 * The encapsulation of malloc(), allocate SIZE bytes of memory,
 * if failed, print error message and abort it 
 */
void *
algmalloc(size_t size)
{
	void *ptr;
	
	if ((ptr = malloc(size)) == NULL)
		errmsg_exit("Memory allocated failure, %s\n", strerror(errno));
	
	return ptr;
}

/* 
 * The encapsulation of calloc(), 
 * allocate NMEMB elements of SIZE bytes each, 
 * all initialized to 0. 
 * if failed, print error message and abort it 
 */
void *
algcalloc(size_t nmemb, size_t size)
{
	void *ptr;
	
	if ((ptr = calloc(nmemb, size)) == NULL)
		errmsg_exit("Memory allocated failure, %s\n", strerror(errno));
	
	return ptr;
}

/* 
 * The encapsulation of realloc(), 
 * re-allocate the previously allocated block in PTR, 
 * making the new block SIZE bytes long.  
 * if failed, print error message and abort it 
 */
void *
algrealloc(void *ptr, size_t size)
{
	void *ret;

	if (!ptr)
		errmsg_exit("Parameters pointer is null.\n");

	if ((ret = realloc(ptr, size)) == NULL)
		errmsg_exit("Memory reallocated failure, %s\n",
			strerror(errno));

	return ret;
}

/* 
 * Returns a random integer-number range from SI to EI - 1.
 */
unsigned int
rand_range_integer(unsigned int si, unsigned int ei)
{
	if (si < ei)
		return si + rand() % (ei - si);
	else if (si > ei)
		return ei + rand() % (si - ei);
	else
		return 0;
}

/* 
 * Returns a random floating-number range from SI to EI - 1.
 */
double
rand_range_float(double si, double ei)
{
	double x = (double)rand() / (double)RAND_MAX;
	
	if (si < ei)
		return si + x * (ei - si);
	else if (si > ei)
		return ei + x * (si - ei);
	else
		return 0.0;
}

/* 
 * Returns a random boolean from a Bernoulli distribution 
 * with success probability p 
 */
bool
bernoulli_distribution(double p)
{
	double x;
	
	if (!(p >= 0.0 && p <= 1.0)) {
		errmsg_exit("probability p must be between 0.0 and 1.0: %f\n",
			p);
	}
	
	x = (double)rand() / (double)RAND_MAX;
	return x < p;
}

/* 
 * Returns a random string that it length is N, 
 * its value need to be free. 
 */
char *
rand_string(short n)
{
	char *str;
	short i;
	
	if (n < 0)
		n = 1;
	else if (n > 1024)
		n = 1024;
	
	str = (char *)algmalloc(sizeof(char) * (n + 1));
	for (i = 0; i < n; i++)
		switch (rand() % 3) {
		case 0:
			*(str + i) = 'A' + rand() % 26;
			break;
		case 1:
			*(str + i) = 'a' + rand() % 26;
			break;
		case 2:
			*(str + i) = '0' + rand() % 10;
			break;
		default:
			*(str + i) = 'A' + rand() % 26;
		}
		
	*(str + i) = '\0';
	
	return str;
}

/* 
 * Rearranges the elements of the specified array 
 * in random integer-number order. 
 */
void
shuffle_uint_array(unsigned int *arr, unsigned int n)
{
	unsigned int i, r, tmp;
	
	for (i = 0; i < n; i++) {
		r = i + rand_range_integer(0, n - i);
		tmp = arr[i];
		arr[i] = arr[r];
		arr[r] = tmp;
	}
}

/* 
 * The encapsulation of fopen(), 
 * open a file and create a new stream for it.
 * if failed, print error message and abort it. 
 */
FILE *
open_file(const char *filename, const char *mode)
{
	FILE *fp;
	
	if ((fp = fopen(filename, mode)) == NULL) {
		errmsg_exit("Can't open file \"%s\", %s\n", filename,
			strerror(errno));
	}
	
	if (setvbuf(fp, NULL, _IOFBF, BUFFER_SIZE) != 0) {
		errmsg_exit("%s file set buffer error, %s\n",
			filename, strerror(errno));
	}
	
	return fp;
}

/* 
 * The encapsulation of fclose(), close STREAM.
 * if failed, print error message and abort it. 
 */
void
close_file(FILE *fp)
{
	if (fflush(fp) != 0) {
		errmsg_exit("Flush buffer data to disk error, %s\n",
			strerror(errno));
	}
	
	if (fclose(fp) != 0)
		errmsg_exit("Closes file error, %s\n", strerror(errno));
}

/* 
 * The encapsulation of strtok(), 
 * divide STR into tokens separated by characters in SEPS.
 */
void
string_tokens(const char *str, const char *seps, char **tokens,
		unsigned short len, unsigned int *sz)
{
	char *ptr, *lstr;
	size_t i, n;
	
	if ((n = strlen(str)) == 0) {
		*sz = 0;
		return;
	}
	
	lstr = (char *)algcalloc(n + 1, sizeof(char));
	strncpy(lstr, str, n);	/* avoid segmentation fault */
	
	ptr = strtok(lstr, seps);
	i = 0;
	while (ptr && i < *sz) {
		strncpy(tokens[i++], ptr, len);
		ptr = strtok(NULL, seps);
	}
	
	ALGFREE(lstr);
	*sz = i;
}

/* erases the end of string of 'enter' */
char *
string_erase_rn(char *str)
{
	size_t len;
	
	if ((len = strlen(str)) == 0)
		return NULL;
	
	if (str[len - 1] == '\n' || str[len - 1] == '\r')
		str[len - 1] = '\0';
	
	if (len >= 2 && str[len - 2] == '\r')
		str[len - 2] = '\0';
	
	return str;
}

/* 
 * Returns the character of string 
 * at the specified index. 
 */
int
string_char_at(const char *str, int d)
{
	long len;

	len = strlen(str);
	if (d >= 0 && d < len)
		return str[d];
	return -1;
}

/* 
 * Removes the char at the specified 
 * position in this string.
 */
char
delete_char_at(char *str, unsigned int i)
{
	char c;
	unsigned int len = strlen(str);

	assert(i < len);
	
	c = *(str + i);
	while (i < len) {
		*(str + i) = *(str + i + 1);
		i++;
	}
		
	return c;
}

/* 
 * Reads one line text from the input stream. 
 * its retrun value need to be free. 
 */
char *
string_read_line(FILE *istream)
{
	char *line, *nline;
	size_t sz = 32, i = 0;
	int c;

	line = (char *)algcalloc(sz + 1, sizeof(char));
	c = fgetc(istream);
	while (c != '\n' && c != EOF) {
		*(line + i++) = (char)c;
		if (i - 1 >= sz / 2) {
			sz += 16;
			nline = (char *)algrealloc(line,
				(sz + 1) * sizeof(char));
			line = nline;
		}
		c = fgetc(istream);
	}

	if (c == (int)'\n') {
		*(line + i) = '\0';
		return line;
	}

	assert(c == EOF);
	ALGFREE(line);
	return NULL;
}

/* 
 * Reads all text from the input stream. 
 * its return value need to be free. 
 */
char *
string_read_all(FILE *istream)
{
	char *context;
	size_t sz, i = 0;
	int c, ahead = (int)' ';

	rewind(istream);
	fseek(istream, 0, SEEK_END);
	sz = ftell(istream);

	context = (char *)algcalloc(sz + 1, sizeof(char));
	rewind(istream);
	while ((c = fgetc(istream)) != EOF) {
		if (isspace(ahead) && isspace(c))
			continue;

		if (isspace(c))
			c = (int)' ';
		*(context + i++) = (char)c;
		ahead = c;
	}

	*(context + i - 1) = '\0';

	return context;
}

/* Returns a substring from a string. */
char *
substring(const char *str, long lo, long hi)
{
	char *subs;
	long i, j;

	subs = (char *)algcalloc(hi - lo + 2, sizeof(char));
	for (i = lo, j = 0; i <= hi; i++, j++)
		*(subs + j) = *(str + i);
	*(subs + j - 1) = '\0';

	return subs;
}
