#include "platform.h"

/*
#ifdef __CONSOLE__
strptr = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, bak, -1, (wchar_t*)string4, STRLEN) - 1;
#else
strptr = mbsrtowcs((wchar_t*)string4, (char const**)&bak, STRLEN, NULL);

if (strptr == -1)
{
strptr = 0;
}

//		strptr++;
#endif


#ifdef __CONSOLE__
pos = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*)name, -1, (wchar_t*)&resbuf[curposbuf], pos);
#else
pos = mbsrtowcs((wchar_t*)&resbuf[curposbuf], (char const**)&name, pos, NULL) + 1;
#endif


*/

#ifndef HAVE_MBSRTOWCS
#include <windows.h>
size_t mbsrtowcs(wchar_t* dest, const char** src, size_t max, void* ps)
{
	return MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, reinterpret_cast<LPCCH>(src), -1, dest, max) - 1;
}
#endif
