// CkSettings.h: interface for the CkSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CKSETTINGS_H__62E2D44A_4858_429D_994D_A6EAAD2FFEA1__INCLUDED_)
#define AFX_CKSETTINGS_H__62E2D44A_4858_429D_994D_A6EAAD2FFEA1__INCLUDED_


#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkSettings  
{
public:
	CkSettings();
	virtual ~CkSettings();

	// CLASS: CkSettings

	// Each Chilkat object's Utf8 property will default to the value of 
	// CkSettings::m_utf8.  The default is false (meaning ANSI).
	static bool m_utf8;  

	// BEGIN PUBLIC INTERFACE

	// Call this once at the beginning of your program
	// if your program is multithreaded.
	static void initializeMultithreaded(void);

	// Call this once at the end of your program.
	// It is only necessary if you are testing for memory leaks.
	static void cleanupMemory(void);


	// Get the sum of the sizes of all the process heaps.
	static unsigned long getTotalSizeProcessHeaps(void);


	static int getAnsiCodePage(void);
	static int getOemCodePage(void);
	static void setAnsiCodePage(int cp);
	static void setOemCodePage(int cp);

	// END PUBLIC INTERFACE
};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif // !defined(AFX_CKSETTINGS_H__62E2D44A_4858_429D_994D_A6EAAD2FFEA1__INCLUDED_)
