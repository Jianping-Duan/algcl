#include "stringsuffixes.h"

/* cutoff to insertion sort */
#define INSERTION_SORT_CUTOFF	8

static inline void exch(struct string_suffixes *, long, long);
static int less(const struct string_suffixes *, long, long, long);
static inline void insertion_sort(struct string_suffixes *, long, long, long);
static void sort(struct string_suffixes *, long, long, long);
static int compare(const struct string_suffixes *, const char *, long);
static long lcp(const struct string_suffixes *, long, long);

/* Initializes a suffix array for the given text string */
void 
strsuffix_init(struct string_suffixes *ss, const char *txt)
{
	long i;

	ss->tlen = strlen(txt);
	ss->text = (char *)algcalloc(ss->tlen + 1, sizeof(char));
	strcpy(ss->text, txt);

	ss->index = (long *)algcalloc(ss->tlen, sizeof(long));
	for (i = 0; i < ss->tlen; i++)
		ss->index[i] = i;
	
	sort(ss, 0, ss->tlen - 1, 0);
}

/* 
 * Returns the length of the longest common prefix of the i th
 * smallest suffix and the i-1st smallest suffix. 
 */
long 
strsuffix_lcp(const struct string_suffixes *ss, long i)
{
	if (i < 1 || i >= ss->tlen)
		errmsg_exit("Index %ld is not between 1 and %ld.\n", i,
			ss->tlen - 1);

	return lcp(ss, ss->index[i], ss->index[i - 1]);
}

/* Returns the ith smallest suffix as a string. */
char * 
strsuffix_select(const struct string_suffixes *ss, long i)
{
	char *str;
	long s, e, j = 0;

	s = ss->index[i];
	e = ss->tlen - ss->index[i];

	str = (char *)algcalloc(e - s + 2, sizeof(char));
	while (s <= e)
		*(str + j++) = ss->text[s++];
	*(str + j) = '\0';	

	return str;
}

/* 
 * Returns the number of suffixes strictly less than the query string.
 * We note that rank(select(i)) equals i for each i between 0 and tlen - 1.
 */
long 
strsuffix_rank(const struct string_suffixes *ss, const char *query)
{
	long lo, mid, hi;
	int cmp;

	lo = 0, hi = ss->tlen - 1;
	while (lo <= hi) {
		mid = lo + (hi - lo) / 2;
		cmp = compare(ss, query, ss->index[mid]);
		if (cmp < 0)
			hi = mid - 1;
		else if (cmp > 0)
			lo = mid + 1;
		else
			return mid;
	}

	return lo;
}

/******************** static function boundary ********************/

/* exchange index[i] and index[j] */
static void 
exch(struct string_suffixes *ss, long i, long j)
{
	long swap;

	swap = ss->index[i];
	ss->index[i] = ss->index[j];
	ss->index[j] = swap;
}

/* is text[i+d..n) < text[j+d..n) ? */
static int 
less(const struct string_suffixes *ss, long i, long j, long d)
{
	if (i == j)
		return 0;
	
	i = i + d;
	j = j + d;

	while (i < ss->tlen && j < ss->tlen) {
		if (ss->text[i] < ss->text[j])
			return 1;
		if (ss->text[i] > ss->text[j])
			return 0;

		i++;
		j++;
	}

	return i > j;
}

/* sort from a[lo] to a[hi], starting at the dth character */
static inline void 
insertion_sort(struct string_suffixes *ss, long lo, long hi, long d)
{
	long i, j;

	for (i = lo; i <= hi; i++)
		for (j = i; j > lo && less(ss, j, j - 1, d); j--)
			exch(ss, j, j - 1);
}

/* 3-way string quicksort lo..hi starting at dth character */
static void 
sort(struct string_suffixes *ss, long lo, long hi, long d)
{
	long lt, gt, i;
	short v, t;

	if (lo + INSERTION_SORT_CUTOFF >= hi) {
		insertion_sort(ss, lo, hi, d);
		return;
	}

	lt = lo, gt = hi;
	i = lo + 1;
	v = ss->text[ss->index[lo] + d];

	while (i <= gt) {
		t = ss->text[ss->index[i] + d];
		if (t < v)
			exch(ss, lt++, i++);
		else if (t > v)
			exch(ss, i, gt--);
		else
			i++;
	}

	sort(ss, lo, lt - 1, d);
	if (v >= 0)
		sort(ss, lt, gt, d + 1);
	sort(ss, gt + 1, hi, d);
}

/* is query < text[i..n) ? */
static int 
compare(const struct string_suffixes *ss, const char *query, long i)
{
	long len, j = 0;

	len = strlen(query);
	while (j < len && i < ss->tlen) {
		if(string_char_at(query, j) != (int)ss->text[i])
			return string_char_at(query, j) - (int)ss->text[i];
		j++;
		i++;
	}

	if (i < ss->tlen)
		return -1;
	if (j < len)
		return 1;
	return 0;
}

/* longest common prefix of text[i..n) and text[j..n) */
static long
lcp(const struct string_suffixes *ss, long i, long j)
{
	long len = 0;

	while (i < ss->tlen && j < ss->tlen) {
		if (ss->text[i] != ss->text[j])
			return len;
		i++;
		j++;
		len++;
	}

	return len;
}
