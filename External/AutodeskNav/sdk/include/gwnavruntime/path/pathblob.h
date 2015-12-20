/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_PathBlob_H
#define Navigation_PathBlob_H

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/path/path.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/channel/channelarrayblob.h"

namespace Kaim
{

class PathBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, PathBlob, 1)

public:
	PathBlob() : m_pathDist(KyFloat32MAXVAL), m_pathCost(KyFloat32MAXVAL) {}
	KyUInt32                  m_visualDebugId;
	KyFloat32                 m_pathDist;
	KyFloat32                 m_pathCost;
	BlobArray<Vec3f>          m_nodePositions;
	BlobRef<ChannelArrayBlob> m_channelArrayBlob;
};
inline void SwapEndianness(Endianness::Target e, PathBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_pathDist);
	SwapEndianness(e, self.m_pathCost);
	SwapEndianness(e, self.m_nodePositions);
	SwapEndianness(e, self.m_channelArrayBlob);
}


class PathBlobBuilder: public BaseBlobBuilder<PathBlob>
{
	KY_CLASS_WITHOUT_COPY(PathBlobBuilder)

public:
	PathBlobBuilder(const Path* path): m_path(path), m_visualDebugId(0) {}
	~PathBlobBuilder() {}

private:
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_visualDebugId, m_visualDebugId);
		if (m_path != NULL && m_path->GetNodeCount() > 0)
		{
			BLOB_SET(m_blob->m_pathDist, m_path->GetPathDistance());
			BLOB_SET(m_blob->m_pathCost, m_path->GetPathCost());
			
			Vec3f* nodePositions = BLOB_ARRAY(m_blob->m_nodePositions, m_path->GetNodeCount());

			if (IsWriteMode())
			{
				for (KyUInt32 i = 0; i < m_path->GetNodeCount(); ++i)
					nodePositions[i] = m_path->GetNodePosition(i);
			}
		}
		/*else
		{
			BLOB_ARRAY(m_blob->m_nodePositions, 0);
		}*/
		if((m_path != KY_NULL) && (m_path->m_channelArray != KY_NULL))
			BUILD_REFERENCED_BLOB(m_blob->m_channelArrayBlob, ChannelArrayBlobBuilder(m_path->m_channelArray, m_visualDebugId));
	}

public:
	const Path* m_path;
	KyUInt32 m_visualDebugId;
};


}

#endif
