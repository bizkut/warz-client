
#pragma once


#define VTUNE_USE



#ifdef VTUNE_USE
	#include "VtuneApi.h"					// c:\Program Files\Intel\VTune\Analyzer\Include
	#pragma comment( lib, "VtuneApi.lib" )	// c:\Program Files\Intel\VTune\Analyzer\Lib

	#define VTunePause		VTPause
	#define VTuneResume		VTResume

	struct TVTuneFuncProfiler
	{
		TVTuneFuncProfiler()
		{
			VTuneResume;
		}

		~TVTuneFuncProfiler()
		{
			VTunePause;
		}
	};

	#define VTuneFuncProfiler( x )	TVTuneFuncProfiler x##_VTuneFuncProfiler;
#endif


