#ifndef _STRINGSORT_H_
#define _STRINGSORT_H_

#include "algcomm.h"

/* Least-significant digit sort */
void lsdsort(char **sa, int w, long n);

/* Most-significant digit sort */
void mstsort(char **sa, int mw, long n);

/* 3-way string quicksort */
void quicksort(char **sa, long n);

/* In-place most-significant digit sort */
void inplace_mstsort(char **sa, long n);

#endif	/* _STRINGSORT_H_ */
