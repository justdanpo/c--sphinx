#include "util.h"
#include <sys/stat.h>
#include <time.h>

long fgetsize(FILE* handle)
{
	long olspos = ftell(handle);

	if (olspos == -1)
	{
		return -1;
	}

	if (fseek(handle, 0, SEEK_END))
	{
		return -1;
	}

	long retsize = ftell(handle);

	if (fseek(handle, olspos, SEEK_SET))
	{
		return -1;
	}

	return retsize;
}


void GetFileTime(char* fname, struct ftime* buf)
{
	struct stat sb;
	struct tm* tblock;

	stat(fname, &sb);
	tblock = localtime(&sb.st_atime);
	buf->ft_tsec = tblock->tm_sec;
	buf->ft_min = tblock->tm_min;
	buf->ft_hour = tblock->tm_hour;
	buf->ft_day = tblock->tm_mday;
	buf->ft_month = tblock->tm_mon;
	buf->ft_year = tblock->tm_year - 80;
}
