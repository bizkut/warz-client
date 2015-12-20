// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "NMPlatform/NMAtomic.h"
#include "NMPlatform/NMBasicLogger.h"
#include "NMPlatform/NMBitArray.h"
#include "NMPlatform/NMBitStreamCoder.h"
#include "NMPlatform/NMBuffer.h"
#include "NMPlatform/NMColour.h"
#include "NMPlatform/NMCommandLineProcessor.h"
#include "NMPlatform/NMDebugDrawManager.h"
#include "NMPlatform/NMFastFreeList.h"
#include "NMPlatform/NMFastHeapAllocator.h"
#include "NMPlatform/NMFile.h"
#include "NMPlatform/NMFlags.h"
#include "NMPlatform/NMFreelistMemoryAllocator.h"
#include "NMPlatform/NMHash.h"
#include "NMPlatform/NMHashMap.h"
#include "NMPlatform/NMHierarchy.h"
#include "NMPlatform/NMKeyboardHarness.h"
#include "NMPlatform/NMLoggingMemoryAllocator.h"
#include "NMPlatform/NMMapContainer.h"
#include "NMPlatform/NMMathPlatform.h"
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMNorm.h"
#include "NMPlatform/NMPadHarness.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMPrioritiesLogger.h"
#include "NMPlatform/NMProfiler.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMRNG.h"
#include "NMPlatform/NMRingBuffer.h"
#include "NMPlatform/NMSocket.h"
#include "NMPlatform/NMSocketWrapper.h"
#include "NMPlatform/NMStaticFreeList.h"
#include "NMPlatform/NMStaticFreeListAllocator.h"
#include "NMPlatform/NMString.h"
#include "NMPlatform/NMStringTable.h"
#include "NMPlatform/NMSync.h"
#include "NMPlatform/NMSystem.h"
#include "NMPlatform/NMTimer.h"
#include "NMPlatform/NMVector.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMVectorContainer.h"

#ifdef _STD
  #error Attempt to include STL in library
#endif
