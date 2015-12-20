#include "r3dPCH.h"
#include "r3d.h"

#include "r3dHash.h"

// using FNV-1a
// See http://www.isthe.com/chongo/tech/comp/fnv/
unsigned r3dHash::MakeHash(const char* pText)
{
	if( !pText || (pText[0] == '\0') )
		return 0;
	else
	{
		// magic number 1
		unsigned uiValue = 0x811c9dc5;
		while ( *pText )
		{
			// xor with case suppressed character
			uiValue ^= ( unsigned )*pText | 0x20;
			// magic number 2
			uiValue *= 0x01000193;
			pText++;
		}

		// return xor-ed with * and multiplied by magic number 2 to differentiate between error and empty string
		return ( uiValue ^ '*' ) * 0x01000193;
	}
}

//////////////////////////////////////////////////////////////////////////

uint32_t r3dHash::MakeHash(const char* buf, size_t count)
{
	if (!buf || count == 0)
		return 0;

	uint32_t uiValue = 0x811c9dc5;
	for (size_t i = 0; i < count; ++i)
	{
		// xor without case suppressed character
		uiValue ^= ( uint32_t )buf[i];
		// magic number 2
		uiValue *= 0x01000193;
	}

	// return xor-ed with * and multiplied by magic number 2 to differentiate between error and empty string
	return ( uiValue ^ '*' ) * 0x01000193;
}