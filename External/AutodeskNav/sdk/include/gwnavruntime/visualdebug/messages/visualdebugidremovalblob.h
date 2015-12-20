/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_VisualDebugIdRemovalBlob_H
#define Navigation_VisualDebugIdRemovalBlob_H

#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"


namespace Kaim
{


/// Send this blob to destroy the current visual debug data of type "blobType" 
/// in the visual debug element with id "visualDebugId".
/// If blobTypeId = KyUInt32MAXVAL, it destroys the whole visual debug element.
class VisualDebugDataRemovalBlob
{
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, VisualDebugDataRemovalBlob, 0)

public:
	VisualDebugDataRemovalBlob(KyUInt32 visualDebugId = KyUInt32MAXVAL, KyUInt32 blobTypeId = KyUInt32MAXVAL)
		: m_visualDebugId(visualDebugId), m_blobTypeId(blobTypeId) {}

	KyUInt32 m_visualDebugId;
	KyUInt32 m_blobTypeId;
};

inline void SwapEndianness(Kaim::Endianness::Target e, VisualDebugDataRemovalBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_blobTypeId);
}



class VisualDebugDataRemovalBlobBuilder : public BaseBlobBuilder<VisualDebugDataRemovalBlob>
{
public:
	VisualDebugDataRemovalBlobBuilder(KyUInt32 visualDebugId, KyUInt32 blobTypeId = KyUInt32MAXVAL)
		: m_visualDebugId(visualDebugId), m_blobTypeId(blobTypeId) {}

	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_visualDebugId, m_visualDebugId);
		BLOB_SET(m_blob->m_blobTypeId, m_blobTypeId);
	}
private:
	KyUInt32 m_visualDebugId;
	KyUInt32 m_blobTypeId;
};

}

#endif // Navigation_VisualDebugIdRemovalBlob_H
