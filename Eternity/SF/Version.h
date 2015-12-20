#pragma once


#ifdef FINAL_BUILD
	#define PROJECT_CONFIG_NAME		"final"
#else
	#ifdef _DEBUG
		#define PROJECT_CONFIG_NAME		"debug"
	#else 
		#define PROJECT_CONFIG_NAME		"release"
	#endif
#endif

extern const char * g_szApplicationName;

extern int32_t	g_nProjectVersionMajor;
extern int32_t	g_nProjectVersionMinor;
extern int32_t	g_nProjectVersionMinor2;

//--------------------------------------------------------------------------------------------------------
#define PROJECT_NAME					"%s v%d.%d.%d  (build: %s %s) - %s"


//--------------------------------------------------------------------------------------------------------
__forceinline const char * GetBuildVersionString()
{
	return Va( PROJECT_NAME, g_szApplicationName, g_nProjectVersionMajor, g_nProjectVersionMinor, g_nProjectVersionMinor2, __DATE__, __TIME__, PROJECT_CONFIG_NAME );
}
