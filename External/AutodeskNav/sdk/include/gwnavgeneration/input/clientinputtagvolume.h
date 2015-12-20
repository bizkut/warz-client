/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LASI - secondary contact: LASI
#ifndef GwNavGen_ClientInputTagVolume_H
#define GwNavGen_ClientInputTagVolume_H

#include "gwnavruntime/database/navtag.h"
#include "gwnavgeneration/generator/generatorblobtyperegistry.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/basesystem/coordsystem.h"

namespace Kaim
{

class ClientInputTagVolumeBlob;
class TagVolume;
class TagVolumeInitConfig;

/// Represents a volume using a two-dimensional polyline extruded vertically along the "up" axis associated to a terrain and/or a BlindData.
/// These volumes may be provided in calls to ClientInputConsumer::ConsumeTagVolume().
/// This class assumes that the coordinates of the input polyline are in Navigation coordinate system.
/// Use ClientInputTagVolume::Init() to setup.
class ClientInputTagVolume
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	/// Creates an unitialized ClientInputTagVolume, that must be set up through a call to Init(). 
	ClientInputTagVolume() {}

	/// Sets up an instance of this class with any required components and configuration parameters. Sets up the dimensions of the volume.
	/// \param points			An array of points that make up the exterior polyline of the volume. This list must be closed: i.e.
	/// 						its last element must be exactly the same as its first member. The points must be specified in
	/// 						clockwise order, seen from above. The polyline may not be twisted: i.e. its edges may not intersect.
	/// \param pointsCount		The number of elements in the points array.
	/// \param altitudeMin		The altitude of the bottom face of the volume.
	/// \param altitudeMax		The altitude of the top face of the volume.
	/// \param blindDataBuffer	A pointer to the associated blindData that will be added to the NavGraph
	/// \param blindDataCount	The number of KyUInt32 the blindData in composed of.
	KyResult Init(
		const Vec3f* points, KyUInt32 pointsCount,
		KyFloat32 altitudeMin, KyFloat32 altitudeMax,
		const KyUInt32* blindDataBuffer=NULL, KyUInt32 blindDataCount=0);
	
	KyResult InitInClientCoordinates(
		const Vec3f* client_points, KyUInt32 pointsCount,
		KyFloat32 client_altitudeMin, KyFloat32 client_altitudeMax,
		const CoordSystem& coordSystem,
		const KyUInt32* blindDataBuffer=NULL, KyUInt32 blindDataCount=0, VisualColor color = DynamicNavTag::GetDefaultVisualColor());
	
	KyResult Init(
		const Vec3f* points, KyUInt32 pointsCount,
		KyFloat32 altitudeMin, KyFloat32 altitudeMax,
		const DynamicNavTag& navTag);

	KyResult InitInClientCoordinates(
		const Vec3f* client_points, KyUInt32 pointsCount,
		KyFloat32 client_altitudeMin, KyFloat32 client_altitudeMax, 
		const DynamicNavTag& navTag,
		const CoordSystem& coordSystem);

	void InitFromBlob(const ClientInputTagVolumeBlob& tagVolumeBlob);

	KyUInt32     GetPointsCount()  const { return m_polyline.GetCount(); } ///< Retrieves the number of unique points in the polyline that outlines this volume.
	KyFloat32    GetAltitudeMin()  const { return m_altMin; } ///< Retrieves the max altitude of the extruded polyline.
	KyFloat32    GetAltitudeMax()  const { return m_altMax; } ///< Retrieves the min altitude of the extruded polyline.

	const Vec3f& GetPoint(KyUInt32 i) const;   ///< Retrieves the point at the specified index within the polyline that outlines this volume.

	void CreateNavigationTagVolume(TagVolume& volume) const; ///< Build a Runtime Tag Volume.
	void CreateNavigationTagVolumeConfig(TagVolumeInitConfig& volumeConfig) const;
public:
	Kaim::KyArray<Vec3f> m_polyline; ///< Stores the polyline set for this volume. Do not modify directly. Access using GetPoint()
	DynamicNavTag m_navTag; //< Stores the NavTag set for this volume.
	KyFloat32 m_altMin; //< The lower altitude limit of the volume.
	KyFloat32 m_altMax; //< The upper altitude limit of the volume.
};


class ClientInputTagVolumeBlob
{
	KY_CLASS_WITHOUT_COPY(ClientInputTagVolumeBlob)
	KY_ROOT_BLOB_CLASS(Generator, ClientInputTagVolumeBlob, 0)
public:
	ClientInputTagVolumeBlob() : m_index(0), m_navTagIdx(KyUInt32MAXVAL) {}

	bool operator==(const ClientInputTagVolumeBlob& other) const
	{
		if (m_polyline.GetCount() != other.m_polyline.GetCount())
		{
			return false;
		}

		const Vec3f* thisPoints = m_polyline.GetValues();
		const Vec3f* otherPoints = other.m_polyline.GetValues();
		for (UPInt i = 0; i < m_polyline.GetCount(); ++i)
		{
			if (thisPoints[i] != otherPoints[i])
			{
				return false;
			}
		}
		return  m_navTag == other.m_navTag && m_altMax == other.m_altMax && m_altMin == other.m_altMin;
	}
	bool operator!=(const ClientInputTagVolumeBlob& other) const { return !operator==(other); }

public:
	BlobArray<Vec3f> m_polyline;
	KyFloat32 m_altMin;
	KyFloat32 m_altMax;
	NavTag m_navTag;
	KyUInt32 m_index;
	mutable KyUInt32 m_navTagIdx;
};
inline void SwapEndianness(Endianness::Target e, ClientInputTagVolumeBlob& self)
{
	SwapEndianness(e, self.m_polyline);
	SwapEndianness(e, self.m_altMin);
	SwapEndianness(e, self.m_altMax);
	SwapEndianness(e, self.m_navTag);
	SwapEndianness(e, self.m_index);
	SwapEndianness(e, self.m_navTagIdx);
}

class ClientInputTagVolumeBlobBuilder : public BaseBlobBuilder<ClientInputTagVolumeBlob>
{
public:
	ClientInputTagVolumeBlobBuilder(const ClientInputTagVolume& tagVolume) : m_tagVolume(&tagVolume) {}
private:
	void DoBuild()
	{
		if (IsWriteMode())
		{
			m_blob->m_index       = 0;
			m_blob->m_altMin	  = m_tagVolume->m_altMin;
			m_blob->m_altMax	  = m_tagVolume->m_altMax;
		}

		BLOB_ARRAY_COPY_2(m_blob->m_polyline, m_tagVolume->m_polyline);
		BLOB_BUILD(m_blob->m_navTag, NavTagBlobBuilder(m_tagVolume->m_navTag));
	}
private:
	const ClientInputTagVolume* m_tagVolume;
};

}


#endif

