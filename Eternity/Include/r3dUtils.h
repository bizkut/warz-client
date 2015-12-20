#ifndef __R3D_UTILS_H
#define __R3D_UTILS_H


	int		random(int);

//
//
// r3dPerfCounter - High Performance Counter, based on P5 RDTSC instruction
//
//
class r3dPerfCounter
{
  public:
	long		perf_StartHi, perf_StartLo, perf_Start;
  public:
	r3dPerfCounter() 
	{ 
 	  Start(); 
	}
	void		Start();
	float		GetDiff();
	long		GetDiffTicks();
	void		GetCurrent(long &Hi, long &Lo);
};

//
//
// r3dFunctionStack
//
//
class r3dFunctionStack
{
  public:
	r3dFunctionStack(char *Name);
	~r3dFunctionStack();
};

#ifdef R3D_DEBUG
 #define FUNCTION_ENTRY(Name)	r3dFunctionStack _local_entry(Name);
#else
 #define FUNCTION_ENTRY(Name)
#endif

//
//
// r3dImageLoader
//
//
class r3dImageLoader
{
  public:
	int		Width;
	int		Height;

	char		*ClrData;		// picture data, RGB format

  public:
			r3dImageLoader();
			~r3dImageLoader();
	int		Load(r3dFile *f);
};


//---------------------------------------------------------------------------------------------
const char * Va( const char * str, ... );

void DumbWCHAR( WCHAR* dest, const char* source ) ;
void DumbANSI( char* dest, const WCHAR* source ) ;


#endif	__R3D_UTILS_H


