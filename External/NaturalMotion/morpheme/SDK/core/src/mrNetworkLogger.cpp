// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetworkLogger.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_NETWORK_LOGGING)
  NMP::PrioritiesLogger g_NetworkLogger;
#endif // defined(MR_NETWORK_LOGGING)

//----------------------------------------------------------------------------------------------------------------------
#if defined(NM_PROFILING)
  NMP::PrioritiesLogger g_ProfilingLogger;
#endif // defined(NM_PROFILING)

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------

