/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: NOBODY
#ifndef Navigation_NewFrameBlob_H
#define Navigation_NewFrameBlob_H


#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"

namespace Kaim
{

	// This class is sent on VisualDebug::NewFrame() calls to send information relative to the frame that ends,
	// and to signify that following blobs are for a new frame.
	// This is an internal blob expected to be handled by the user of the VisualDebugClient
	class FrameBlob
	{
		KY_ROOT_BLOB_CLASS(VisualDebugMessage, FrameBlob, 2)
	public:

		FrameBlob()
			: m_frameNumber(0)
			, m_simulationTimeInSeconds(0.f)
			, m_gameFrameTimeInSeconds(0.f)
		{}

		KyUInt32 m_frameNumber;
		KyFloat32 m_simulationTimeInSeconds;
		KyFloat32 m_gameFrameTimeInSeconds;
	};

	inline void SwapEndianness(Endianness::Target e, FrameBlob& self)
	{
		SwapEndianness(e, self.m_frameNumber);
		SwapEndianness(e, self.m_simulationTimeInSeconds);
		SwapEndianness(e, self.m_gameFrameTimeInSeconds);
	}

} // namespace Kaim

#endif // Navigation_NewFrameBlob_H
