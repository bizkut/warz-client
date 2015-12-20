#ifndef __COMMON_MAXINCLUDES_H
#define __COMMON_MAXINCLUDES_H

#undef STRICT
#define STRICT

#if 0
// memory mumbo-jumbo
#include "coreexp.h"
#undef _CRTDBG_CHECK_DEFAULT_DF
#undef _malloc_dbg
#undef _calloc_dbg
#undef _realloc_dbg
#undef _expand_dbg
#undef _free_dbg
#undef _msize_dbg
#include "MAX_Mem.h"	
#endif

#include "max.h"
#include "UtilApi.h"
#include "stdmat.h"
#include "decomp.h"
#include "shape.h"
#include "simpobj.h"
#include "interpik.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "maxscrpt/maxscrpt.h"


#if MAX_RELEASE >= 4000
#include "iskin.h"
#endif

#if 0
 // use Chracter Studio 3.2 includes
 #include "cs32_bipexp.h"
 #include "cs32_phyexp.h"
 // this is the class for all biped controllers except the root and the footsteps
 #define BIPSLAVE_CONTROL_CLASS_ID Class_ID(0x9154,0)
 // this is the class for the center of mass, biped root controller ("Bip01")
 #define BIPBODY_CONTROL_CLASS_ID  Class_ID(0x9156,0) 
#else
 // use 3DSMax7 Character Studio SDK
 #include "cs\BipExp.h"
 #include "cs\PhyExp.h"
 #include "cs\BipedApi.h"
#endif


	//
	// 3DSMax Libraries
	//

#pragma comment(lib, "core.lib")
#pragma comment(lib, "geom.lib")
#pragma comment(lib, "gfx.lib")
#pragma comment(lib, "bmm.lib")
#pragma comment(lib, "mesh.lib")
#pragma comment(lib, "maxutil.lib")
#pragma comment(lib, "maxscrpt.lib")
#pragma comment(lib, "gup.lib")
#pragma comment(lib, "paramblk2.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "particle.lib")
#pragma comment(lib, "assetmanagement.lib")

#endif	// __COMMON_MAXINCLUDES_H
