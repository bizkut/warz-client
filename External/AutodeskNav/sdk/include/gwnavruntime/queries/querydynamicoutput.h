/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_QueryDynamicOutput_H
#define Navigation_QueryDynamicOutput_H

#include "gwnavruntime/queries/utils/navtagsubsegment.h"
#include "gwnavruntime/queries/utils/halfedgeintersection.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/spatialization/spatializedpoint.h"
#include "gwnavruntime/world/tagvolume.h"


namespace Kaim
{


/// Enumerates possible ways of storing crossed data from a query in a QueryDynamicOutput object.
enum DynamicOutputMode
{
	QUERY_SAVE_NOTHING                 = 0,      ///< Indicates that QueryDynamicOutput will not be used to store anything.
	QUERY_SAVE_FLOORS                  = 1 << 1, ///< Indicates that QueryDynamicOutput will be used to store NavFloorPtr.
	QUERY_SAVE_TRIANGLES               = 1 << 2, ///< Indicates that QueryDynamicOutput will be used to store NavFloorTrianglePtr.
	QUERY_SAVE_NAVTAGSUBSEGMENTS       = 1 << 3, ///< Indicates that QueryDynamicOutput will be used to store NavTagSubSegment.
	QUERY_SAVE_HALFEDGEINTERSECTIONS   = 1 << 4, ///< Indicates that QueryDynamicOutput will be used to store Vec3f.

	QUERY_SAVE_TRIANGLES_AND_SUBSEGMENTS = QUERY_SAVE_TRIANGLES | QUERY_SAVE_NAVTAGSUBSEGMENTS,
};


template <class T>
class WorkingMemArray;

template <class T>
class QueryOutputBuffer_Impl
{
public:
	QueryOutputBuffer_Impl() : m_buffer(KY_NULL), m_count(0) {}
	void ClearBuffer();
	void InitBuffer(KyUInt32 count, char*& memory);
	KyUInt32 GetCount() const;
	const T& Get(KyUInt32 index) const;
	T& Get(KyUInt32 index);
	void Set(KyUInt32 index, const T& object);

	T* m_buffer;
	KyUInt32 m_count;
};



/// This class gather the data encountered along some query process.
class QueryDynamicOutput
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QueryOutput)
	QueryDynamicOutput();

public:
	class Config
	{
	public:
		Config();
	public: 
		KyUInt32 m_floorCount;
		KyUInt32 m_triangleCount;
		KyUInt32 m_subSegmentCount;
		KyUInt32 m_halfEdgeIntersectionCount;
		KyUInt32 m_spatializedPointCount;
		KyUInt32 m_tagVolumeCount;
	};

	// ---------------------------------- Main API functions ----------------------------------

	KyUInt32 GetNavFloorPtrCount()          const;
	KyUInt32 GetNavTrianglePtrCount()       const;
	KyUInt32 GetNavTagSubSegmentCount()     const;
	KyUInt32 GetHalfEdgeIntersectionCount() const;
	KyUInt32 GetSpatializedPointCount()     const;
	KyUInt32 GetTagVolumeCount()            const;

	const NavTrianglePtr& GetNavTrianglePtr(KyUInt32 trianglePtrIndex) const;
	      NavTrianglePtr& GetNavTrianglePtr(KyUInt32 trianglePtrIndex);

	const NavFloorPtr& GetNavFloorPtr(KyUInt32 floorPtrIndex) const;
	      NavFloorPtr& GetNavFloorPtr(KyUInt32 floorPtrIndex);

	const NavTagSubSegment& GetNavTagSubSegment(KyUInt32 navTagSubSegmentIndex) const;
	      NavTagSubSegment& GetNavTagSubSegment(KyUInt32 navTagSubSegmentIndex);

	const HalfEdgeIntersection& GetHalfEdgeIntersection(KyUInt32 halfEdgeIntersectionIndex) const;
	      HalfEdgeIntersection& GetHalfEdgeIntersection(KyUInt32 halfEdgeIntersectionIndex);

	const Ptr<SpatializedPoint>& GetSpatializedPoint(KyUInt32 spatializedPointIndex) const;
	      Ptr<SpatializedPoint>& GetSpatializedPoint(KyUInt32 spatializedPointIndex);

	const Ptr<TagVolume>& GetTagVolume(KyUInt32 tagVolumeIdx) const;
	      Ptr<TagVolume>& GetTagVolume(KyUInt32 tagVolumeIdx);

	void SetNavTrianglePtr      (KyUInt32 trianglePtrIndex,          const NavTrianglePtr& navTrianglePtr);
	void SetNavTagSubSegment    (KyUInt32 navTagSubSegmentIndex,     const NavTagSubSegment& navTagSubSegment);
	void SetHalfEdgeIntersection(KyUInt32 halfEdgeIntersectionIndex, const HalfEdgeIntersection& halfEdgeIntersection);
	void SetNavFloorPtr         (KyUInt32 floorPtrIndex,             const NavFloorPtr& navFloorPtr);
	void SetSpatializedPoint    (KyUInt32 spatializedPointIndex,     const Ptr<SpatializedPoint>& spatializedPoint);
	void SetTagVolume           (KyUInt32 tagVolumeIndex,            const Ptr<TagVolume>& tagVolume);

	static KyUInt32 ComputeByteSize(const Config& config);
	static Ptr<QueryDynamicOutput> Create(const Config& config);

	void ClearBuffers();
	bool IsEnoughMemoryForStoringDynamicOutput(const Config& config);
	void InitBuffers( const Config& config);

	bool IsEmpty() const;

private:
	QueryOutputBuffer_Impl<NavFloorPtr           > m_navFloorPtrs;
	QueryOutputBuffer_Impl<NavTrianglePtr        > m_navTrianglePtrs;
	QueryOutputBuffer_Impl<NavTagSubSegment      > m_navTagSubSegments;
	QueryOutputBuffer_Impl<HalfEdgeIntersection  > m_halfEgdeIntersections;
	QueryOutputBuffer_Impl<Ptr<SpatializedPoint> > m_spatializedPoints;
	QueryOutputBuffer_Impl<Ptr<TagVolume>        > m_tagVolumes;

	KyUInt32 m_byteSize;
	KyUInt32 RefCount; // consider thread-safetyness here with mutable AtomicInt<int> RefCount;


public:
	// Lifetime - separate implementation due to Create function
	KY_INLINE void AddRef() { RefCount ++; }
	KY_INLINE void Release()
	{
		if (RefCount-- == 1)
		{
			ClearBuffers();
			KY_FREE(this);
		}
	}

	// Debug method only.
	int GetRefCount() const { return RefCount; }
};

class ScopeAutoSaveDynamicOutput
{
public:
	ScopeAutoSaveDynamicOutput(Ptr<QueryDynamicOutput>& dynamicOutput);
	~ScopeAutoSaveDynamicOutput();

	void SetNavFloorRawPtrs(WorkingMemArray<NavFloorRawPtr>* navFloorRawPtrs);
	void SetNavTriangleRawPtrs(WorkingMemArray<NavTriangleRawPtr>* navTriangleRawPtrs);
	void SetRawNavTagSubSegments(WorkingMemArray<RawNavTagSubSegment>* rawNavTagSubSegments);
	void SetRawHalfEdgeIntersections(WorkingMemArray<RawHalfEdgeIntersection>* rawHalfEdgeIntersections);
	void SetSpatializedPoints(WorkingMemArray<SpatializedPoint*>* spatializedPoints);
	void SetTagVolumes(WorkingMemArray<TagVolume*>* tagVolumes);
private:
	void CopyCrossedDataIntoDynamicOutput();

	Ptr<QueryDynamicOutput>*                  m_queryDynamicOutput;
	WorkingMemArray<NavFloorRawPtr>*          m_navFloorRawPtrs;
	WorkingMemArray<NavTriangleRawPtr>*       m_navTriangleRawPtrs;
	WorkingMemArray<RawNavTagSubSegment>*     m_rawNavTagSubSegments;
	WorkingMemArray<RawHalfEdgeIntersection>* m_rawHalfEdgeIntersections;
	WorkingMemArray<SpatializedPoint*>*       m_spatializedPoints;
	WorkingMemArray<TagVolume*>*              m_tagVolumes;
};


} // namespace Kaim


#include "gwnavruntime/queries/querydynamicoutput.inl"

#endif //Navigation_QueryDynamicOutput_H

