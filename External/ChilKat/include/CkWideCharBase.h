#ifndef _CkWideCharBase_H
#define _CkWideCharBase_H
#pragma once

#ifndef __sun__
#pragma pack (push, 8)
#endif

#include "CkString.h"

class CkWideCharBase
{
    private:
		
	// Disallow assignment or copying this object.
	CkWideCharBase(const CkWideCharBase &) { }
	CkWideCharBase &operator=(const CkWideCharBase &) { return *this; }

    protected:
		
	bool m_forMono;

	unsigned int m_resultIdxW;
	CkString *m_pResultStringW[10];

	unsigned int nextIdxW(void);
	
	const wchar_t *rtnWideString(const char *strUtf8);
	
    public:
		
	CkWideCharBase();
	virtual ~CkWideCharBase();
	

    };

#ifndef __sun__
#pragma pack (pop)
#endif

#endif
	
