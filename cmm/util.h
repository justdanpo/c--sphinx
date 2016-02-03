#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
long fgetsize(FILE* handle);

struct ftime
{
	unsigned ft_tsec: 5; /* две секунды */
	unsigned ft_min: 6;  /* минуты */
	unsigned ft_hour: 5; /* часы */
	unsigned ft_day: 5;  /* день */
	unsigned ft_month: 4; /* месяц */
	unsigned ft_year: 7; /* год-1980 */
};

void GetFileTime(char* fname, struct ftime* buf);

#endif
