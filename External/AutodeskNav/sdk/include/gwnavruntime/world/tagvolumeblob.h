/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_TagVolumeBlob_H
#define Navigation_TagVolumeBlob_H

#include "gwnavruntime/world/tagvolume.h"
#include "gwnavruntime/world/databasebindingblob.h"


namespace Kaim
{

class TagVolumeBlob
{
	KY_CLASS_WITHOUT_COPY(TagVolumeBlob)
	KY_ROOT_BLOB_CLASS(NavData, TagVolumeBlob, 106)

public:
	TagVolumeBlob() : m_navTagIdx(KyUInt32MAXVAL) {}

	bool operator==(const TagVolumeBlob& other) const
	{
		if ((m_points.GetCount() != other.m_points.GetCount()) ||
			(m_aabb              != other.m_aabb) ||
			(m_obstacleType      != other.m_obstacleType) ||
			(m_navTag            != other.m_navTag))
		{
			return false;
		}

		const Vec2f* thisPoints = m_points.GetValues();
		const Vec2f* otherPoints = other.m_points.GetValues();
		for (KyUInt32 i = 0; i < m_points.GetCount(); ++i)
		{
			if (thisPoints[i] != otherPoints[i])
				return false;
		}

		return true;
	}
	bool operator!=(const TagVolumeBlob& other) const { return !operator==(other); }

	TagVolume::ObstacleType GetObstacleType() const { return (TagVolume::ObstacleType)m_obstacleType; }
	TagVolumeExtendOptions GetTagVolumeExtendOptions() const { return (TagVolumeExtendOptions)m_tagVolumeExtendOptions; }
public:
	KyUInt32 m_visualDebugId;
	BlobArray<Vec2f> m_points;
	Box3f m_aabb;
	KyUInt16 m_obstacleType; // cast this into TagVolume::ObstacleType
	KyUInt16 m_tagVolumeExtendOptions; //TagVolumeExtendOptions
	NavTag m_navTag;
	mutable KyUInt32 m_navTagIdx;
	DatabaseBindingBlob m_databaseBindings;
};

inline void SwapEndianness(Endianness::Target e, TagVolumeBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_points);
	SwapEndianness(e, self.m_aabb);
	SwapEndianness(e, self.m_obstacleType);
	SwapEndianness(e, self.m_tagVolumeExtendOptions);
	SwapEndianness(e, self.m_navTag);
	SwapEndianness(e, self.m_navTagIdx);
	SwapEndianness(e, self.m_databaseBindings);
}

class TagVolumeBlobBuilder : public BaseBlobBuilder<TagVolumeBlob>
{
public:
	TagVolumeBlobBuilder(const TagVolume* tagVolume) : m_tagVolume(tagVolume) {}

private:
	void DoBuild()
	{
		if (m_tagVolume)
		{
			BLOB_SET(m_blob->m_visualDebugId, m_tagVolume->GetVisualDebugId());
			BLOB_SET(m_blob->m_aabb, m_tagVolume->GetAABB());
			BLOB_SET(m_blob->m_obstacleType, (KyUInt16)m_tagVolume->GetObstacleType());
			BLOB_SET(m_blob->m_tagVolumeExtendOptions, (KyUInt16)m_tagVolume->GetTagVolumeExtendOptions());
			BLOB_ARRAY_COPY(m_blob->m_points, m_tagVolume->GetPoints(), m_tagVolume->GetPointCount());
			BLOB_BUILD(m_blob->m_navTag, NavTagBlobBuilder(m_tagVolume->GetNavTag()));
			BLOB_BUILD(m_blob->m_databaseBindings, DatabaseBindingBlobBuilder(m_tagVolume->GetDatabaseBinding()));
		}
	}

	const TagVolume* m_tagVolume;
};

class TagVolumeContextBlob
{
	KY_CLASS_WITHOUT_COPY(TagVolumeContextBlob)
	KY_ROOT_BLOB_CLASS(NavData, TagVolumeContextBlob, 0)

public:
	TagVolumeContextBlob() {}

	TagVolume::IntegrationStatus GetIntegrationStatus() const { return (TagVolume::IntegrationStatus)m_integrationStatus; }
	TagVolume::WorldStatus       GetWorldStatus()       const { return (TagVolume::WorldStatus)m_worldStatus;             }
public:
	KyUInt32 m_visualDebugId;
	KyUInt16 m_integrationStatus; // cast this into IntegratonStatus
	KyUInt16 m_worldStatus; // cast this into WorldStatus;
};

inline void SwapEndianness(Endianness::Target e, TagVolumeContextBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_integrationStatus);
	SwapEndianness(e, self.m_worldStatus);
}

class TagVolumeContextBlobBuilder : public BaseBlobBuilder<TagVolumeContextBlob>
{
public:
	TagVolumeContextBlobBuilder(const TagVolume* tagVolume) : m_tagVolume(tagVolume) {}

private:
	void DoBuild()
	{
		if (m_tagVolume)
		{
			BLOB_SET(m_blob->m_visualDebugId, m_tagVolume->GetVisualDebugId());
			BLOB_SET(m_blob->m_integrationStatus, (KyUInt16)m_tagVolume->GetIntegrationStatus());
			BLOB_SET(m_blob->m_worldStatus,       (KyUInt16)m_tagVolume->GetWorldStatus());
		}
	}

	const TagVolume* m_tagVolume;
};

} // namespace Kaim

#endif // Navigation_TagVolumeBlob_H
