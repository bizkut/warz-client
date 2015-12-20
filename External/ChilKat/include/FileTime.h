
#if !defined(WIN32) && !defined(WINCE)

#ifndef _FILETIME_H_INCLUDED_
#define _FILETIME_H_INCLUDED_
	
typedef struct _FILETIME
    {
    unsigned long dwLowDateTime;
    unsigned long dwHighDateTime;
    } 	FILETIME;

#endif
	

#endif

