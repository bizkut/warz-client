/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


namespace Kaim
{


template <class T>
KY_INLINE void QueryOutputBuffer_Impl<T>::ClearBuffer()
{
	ConstructorCPP<T>::DestructArray(m_buffer, m_count);
	m_buffer = KY_NULL;
	m_count = 0;
}

template <class T>
KY_INLINE void QueryOutputBuffer_Impl<T>::InitBuffer(KyUInt32 count, char*& memory)
{
	if (count != 0)
	{
		m_count = count;
		m_buffer = (T*)memory;
		memory += count * sizeof(T);
		ConstructorCPP<T>::ConstructArray(m_buffer, count);
	}
	else
	{
		m_buffer = KY_NULL;
		m_count = 0;
	}
}

template <class T>
KY_INLINE KyUInt32 QueryOutputBuffer_Impl<T>::GetCount() const { return m_count; }

template <class T>
KY_INLINE const T& QueryOutputBuffer_Impl<T>::Get(KyUInt32 index) const
{
	KY_ASSERT(index < m_count);
	return m_buffer[index];
}


template <class T>
KY_INLINE T& QueryOutputBuffer_Impl<T>::Get(KyUInt32 index)
{
	KY_ASSERT(index < m_count);
	return m_buffer[index];
}

template <class T>
KY_INLINE void QueryOutputBuffer_Impl<T>::Set(KyUInt32 index, const T& object)
{
	KY_ASSERT(index < m_count);
	m_buffer[index] = object;
}


KY_INLINE QueryDynamicOutput::QueryDynamicOutput() : m_byteSize(0), RefCount(1) {}

KY_INLINE QueryDynamicOutput::Config::Config()
	: m_floorCount(0)
	, m_triangleCount(0)
	, m_subSegmentCount(0)
	, m_halfEdgeIntersectionCount(0)
	, m_spatializedPointCount(0)
	, m_tagVolumeCount(0)
{}

KY_INLINE bool QueryDynamicOutput::IsEnoughMemoryForStoringDynamicOutput(const Config& config)
{
	return ComputeByteSize(config) <= m_byteSize;
}

KY_INLINE KyUInt32 QueryDynamicOutput::GetNavFloorPtrCount()          const { return m_navFloorPtrs.GetCount();           }
KY_INLINE KyUInt32 QueryDynamicOutput::GetNavTrianglePtrCount()       const { return m_navTrianglePtrs.GetCount();        }
KY_INLINE KyUInt32 QueryDynamicOutput::GetNavTagSubSegmentCount()     const { return m_navTagSubSegments.GetCount();      }
KY_INLINE KyUInt32 QueryDynamicOutput::GetHalfEdgeIntersectionCount() const { return m_halfEgdeIntersections.GetCount();  }
KY_INLINE KyUInt32 QueryDynamicOutput::GetSpatializedPointCount()     const { return m_spatializedPoints.GetCount();      }
KY_INLINE KyUInt32 QueryDynamicOutput::GetTagVolumeCount()            const { return m_tagVolumes.GetCount();             }

KY_INLINE const NavFloorPtr&           QueryDynamicOutput::GetNavFloorPtr(KyUInt32 idx)          const { return m_navFloorPtrs.Get(idx);          }
KY_INLINE       NavFloorPtr&           QueryDynamicOutput::GetNavFloorPtr(KyUInt32 idx)                { return m_navFloorPtrs.Get(idx);          }
KY_INLINE const NavTrianglePtr&        QueryDynamicOutput::GetNavTrianglePtr(KyUInt32 idx)       const { return m_navTrianglePtrs.Get(idx);       }
KY_INLINE       NavTrianglePtr&        QueryDynamicOutput::GetNavTrianglePtr(KyUInt32 idx)             { return m_navTrianglePtrs.Get(idx);       }
KY_INLINE const NavTagSubSegment&      QueryDynamicOutput::GetNavTagSubSegment(KyUInt32 idx)     const { return m_navTagSubSegments.Get(idx);     }
KY_INLINE       NavTagSubSegment&      QueryDynamicOutput::GetNavTagSubSegment(KyUInt32 idx)           { return m_navTagSubSegments.Get(idx);     }
KY_INLINE const HalfEdgeIntersection&  QueryDynamicOutput::GetHalfEdgeIntersection(KyUInt32 idx) const { return m_halfEgdeIntersections.Get(idx); }
KY_INLINE       HalfEdgeIntersection&  QueryDynamicOutput::GetHalfEdgeIntersection(KyUInt32 idx)       { return m_halfEgdeIntersections.Get(idx); }
KY_INLINE const Ptr<SpatializedPoint>& QueryDynamicOutput::GetSpatializedPoint(KyUInt32 idx)     const { return m_spatializedPoints.Get(idx);     }
KY_INLINE       Ptr<SpatializedPoint>& QueryDynamicOutput::GetSpatializedPoint(KyUInt32 idx)           { return m_spatializedPoints.Get(idx);     }
KY_INLINE const Ptr<TagVolume>&        QueryDynamicOutput::GetTagVolume(KyUInt32 idx)            const { return m_tagVolumes.Get(idx);            }
KY_INLINE       Ptr<TagVolume>&        QueryDynamicOutput::GetTagVolume(KyUInt32 idx)                  { return m_tagVolumes.Get(idx);            }

KY_INLINE void QueryDynamicOutput::SetNavFloorPtr(KyUInt32 idx, const NavFloorPtr& navFloorPtr)                            { m_navFloorPtrs.Set(idx, navFloorPtr);                   }
KY_INLINE void QueryDynamicOutput::SetNavTrianglePtr(KyUInt32 idx, const NavTrianglePtr& navTrianglePtr)                   { m_navTrianglePtrs.Set(idx, navTrianglePtr);             }
KY_INLINE void QueryDynamicOutput::SetNavTagSubSegment(KyUInt32 idx, const NavTagSubSegment& navTagSubSegment)             { m_navTagSubSegments.Set(idx, navTagSubSegment);         }
KY_INLINE void QueryDynamicOutput::SetHalfEdgeIntersection(KyUInt32 idx, const HalfEdgeIntersection& halfEdgeIntersection) { m_halfEgdeIntersections.Set(idx, halfEdgeIntersection); }
KY_INLINE void QueryDynamicOutput::SetSpatializedPoint(KyUInt32 idx, const Ptr<SpatializedPoint>& spatializedPoint)        { m_spatializedPoints.Set(idx, spatializedPoint);         }
KY_INLINE void QueryDynamicOutput::SetTagVolume(KyUInt32 idx, const Ptr<TagVolume>& tagVolume)                             { m_tagVolumes.Set(idx, tagVolume);                       }

KY_INLINE ScopeAutoSaveDynamicOutput::ScopeAutoSaveDynamicOutput(Ptr<QueryDynamicOutput>& dynamicOutput)
	: m_queryDynamicOutput(&dynamicOutput)
	, m_navFloorRawPtrs(KY_NULL)
	, m_navTriangleRawPtrs(KY_NULL)
	, m_rawNavTagSubSegments(KY_NULL)
	, m_rawHalfEdgeIntersections(KY_NULL)
	, m_spatializedPoints(KY_NULL)
	, m_tagVolumes(KY_NULL)
{}

KY_INLINE void ScopeAutoSaveDynamicOutput::SetNavFloorRawPtrs(WorkingMemArray<NavFloorRawPtr>* navFloorRawPtrs)                            { m_navFloorRawPtrs          = navFloorRawPtrs;          }
KY_INLINE void ScopeAutoSaveDynamicOutput::SetNavTriangleRawPtrs(WorkingMemArray<NavTriangleRawPtr>* navTriangleRawPtrs)                   { m_navTriangleRawPtrs       = navTriangleRawPtrs;       }
KY_INLINE void ScopeAutoSaveDynamicOutput::SetRawNavTagSubSegments(WorkingMemArray<RawNavTagSubSegment>* rawNavTagSubSegments)             { m_rawNavTagSubSegments     = rawNavTagSubSegments;     }
KY_INLINE void ScopeAutoSaveDynamicOutput::SetRawHalfEdgeIntersections(WorkingMemArray<RawHalfEdgeIntersection>* rawHalfEdgeIntersections) { m_rawHalfEdgeIntersections = rawHalfEdgeIntersections; }
KY_INLINE void ScopeAutoSaveDynamicOutput::SetSpatializedPoints(WorkingMemArray<SpatializedPoint*>* spatializedPoints)                     { m_spatializedPoints        = spatializedPoints;        }
KY_INLINE void ScopeAutoSaveDynamicOutput::SetTagVolumes(WorkingMemArray<TagVolume*>* tagVolumes)                                          { m_tagVolumes               = tagVolumes;               }

KY_INLINE ScopeAutoSaveDynamicOutput::~ScopeAutoSaveDynamicOutput()
{
	if (m_navFloorRawPtrs          != KY_NULL || m_navTriangleRawPtrs != KY_NULL || m_rawNavTagSubSegments != KY_NULL ||
		m_rawHalfEdgeIntersections != KY_NULL || m_spatializedPoints  != KY_NULL || m_tagVolumes           != KY_NULL )
	{
		CopyCrossedDataIntoDynamicOutput();
	}
}

} // namespace Kaim
