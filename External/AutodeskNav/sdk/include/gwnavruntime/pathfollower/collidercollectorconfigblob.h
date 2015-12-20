/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: MAMU
#ifndef Navigation_ColliderCollectorConfigBlob_H
#define Navigation_ColliderCollectorConfigBlob_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/pathfollower/collidercollectorconfig.h"

namespace Kaim
{


class ColliderCollectorConfigBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, ColliderCollectorConfigBlob, 0)

public:
	KyUInt32 m_visualDebugId;
	KyFloat32 m_colliderCollectorRadius;
	KyFloat32 m_colliderCollectorHalfHeight;
	KyUInt32 m_colliderCollectorFramesBetweenUpdates;
};

inline void SwapEndianness(Endianness::Target e, ColliderCollectorConfigBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_colliderCollectorRadius);
	SwapEndianness(e, self.m_colliderCollectorHalfHeight);
	SwapEndianness(e, self.m_colliderCollectorFramesBetweenUpdates);
};


class ColliderCollectorConfigBlobBuilder: public BaseBlobBuilder<ColliderCollectorConfigBlob>
{
	KY_CLASS_WITHOUT_COPY(ColliderCollectorConfigBlobBuilder)

public:
	ColliderCollectorConfigBlobBuilder(const ColliderCollectorConfig* colliderCollectorConfig, KyUInt32 visualDebugId)
		: m_colliderCollectorConfig(colliderCollectorConfig), m_visualDebugId(visualDebugId) {}

	~ColliderCollectorConfigBlobBuilder() {}

private:
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_visualDebugId, m_visualDebugId);
		BLOB_SET(m_blob->m_colliderCollectorRadius              , m_colliderCollectorConfig->m_colliderCollectorRadius);
		BLOB_SET(m_blob->m_colliderCollectorHalfHeight          , m_colliderCollectorConfig->m_colliderCollectorHalfHeight);
		BLOB_SET(m_blob->m_colliderCollectorFramesBetweenUpdates, m_colliderCollectorConfig->m_colliderCollectorFramesBetweenUpdates);
	}

	const ColliderCollectorConfig* m_colliderCollectorConfig;
	KyUInt32 m_visualDebugId;
};

} // namespace Kaim

#endif // Navigation_ColliderCollectorConfigBlob_H
