/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_PositionOnPathBlob_H
#define Navigation_PositionOnPathBlob_H

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/path/positiononpath.h"
#include "gwnavruntime/path/livepath.h"
#include "gwnavruntime/blob/baseblobbuilder.h"

namespace Kaim
{

class PositionOnPathBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, PositionOnPathBlob, 0)

public:
	PositionOnPathBlob() : m_edgeIndex(KyUInt32MAXVAL), m_onPathStatus((KyUInt32)PositionOnPathStatus_Undefined) {}

	PositionOnPathStatus GetPositionStatus() const { return (PositionOnPathStatus)m_onPathStatus; }
public:
	Vec3f m_positionOnPath;
	KyUInt32 m_edgeIndex;
	KyUInt32 m_onPathStatus;
};
inline void SwapEndianness(Endianness::Target e, PositionOnPathBlob& self)
{
	SwapEndianness(e, self.m_positionOnPath);
	SwapEndianness(e, self.m_edgeIndex);
	SwapEndianness(e, self.m_onPathStatus);
}


class PositionOnPathBlobBuilder: public BaseBlobBuilder<PositionOnPathBlob>
{
	KY_CLASS_WITHOUT_COPY(PositionOnPathBlobBuilder)

public:
	PositionOnPathBlobBuilder(const PositionOnPath* positionOnPath)
		: m_positionOnPath(positionOnPath)
	{}
	~PositionOnPathBlobBuilder() {}

private:
	virtual void DoBuild()
	{
		if ((m_positionOnPath != NULL) && (m_positionOnPath->GetPositionOnPathStatus() != PositionOnPathStatus_Undefined))
		{
			BLOB_SET(m_blob->m_positionOnPath, m_positionOnPath->GetPosition());
			if (m_positionOnPath->GetPositionOnPathStatus() != PositionOnPathStatus_Undefined)
			{
				BLOB_SET(m_blob->m_edgeIndex , m_positionOnPath->GetPathEdgeIndex());
			}
			BLOB_SET(m_blob->m_onPathStatus, (KyUInt32)m_positionOnPath->GetPositionOnPathStatus());
		}
	}

private:
	const PositionOnPath* m_positionOnPath;
};


}

#endif
