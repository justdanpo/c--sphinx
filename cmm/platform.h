#ifndef __PLATFORM_H__

#include "config.h"

#include <string.h>
#if defined(HAVE_STRCASECMP)
#elif defined(HAVE__STRICMP)
#define strcasecmp _stricmp
#elif defined(HAVE_STRICMP)
#define strcasecmp stricmp
#else
#error no strcasecmp
#endif

#if defined(HAVE_STRNCASECMP)
#elif defined(HAVE__STRNICMP)
#define strncasecmp _strnicmp
#elif defined(HAVE_STRNICMP)
#define strncasecmp strnicmp
#else
#error no strncasecmp
#endif


#if defined(HAVE_MBSRTOWCS)
#include <wchar.h>
#else
size_t mbsrtowcs(wchar_t* dest, const char** src, size_t max, void* ps);//mbstate_t* ps
#endif

#endif
