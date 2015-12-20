/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_DiagonalStrip_H
#define Navigation_DiagonalStrip_H

#include "gwnavruntime/channel/bubble.h"

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/containers/sharedpoollist.h"
#include "gwnavruntime/visualsystem/displaylistblob.h"


namespace Kaim
{

class World;

//class DisplayListToggle
//{
//public:
//	enum ToggleValue
//	{
//		ShowByDefault,
//		SendButDontShow,
//		DontSend
//	};
//
//	DisplayListToggle()
//		: m_toggleValue(ShowByDefault)
//	{}
//
//	bool MustBeSent()     const { return (m_toggleValue != DontSend); }
//	bool MustNotBeShown() const { return (m_toggleValue != ShowByDefault); }
//
//	ToggleValue m_toggleValue;
//};

//////////////////////////////////////////////////////////////////////////
//
//  DiagonalStripDisplayConfig
//
//////////////////////////////////////////////////////////////////////////
class DiagonalStripPersistentDisplayListIds
{
public:
	DiagonalStripPersistentDisplayListIds()
		: m_world(KY_NULL)
		, m_singleFrame(true)
		, m_displayListId_Left(KyUInt32MAXVAL)
		, m_displayListId_Right(KyUInt32MAXVAL)
		, m_displayListId_Corners(KyUInt32MAXVAL)
		, m_displayListId_NearestOpposite(KyUInt32MAXVAL)
		, m_displayListId_DiskToNearestOpposite(KyUInt32MAXVAL)
		, m_displayListId_Diagonals(KyUInt32MAXVAL)
		, m_displayListId_CornerIdx(KyUInt32MAXVAL)
		, m_displayListId_Triangles(KyUInt32MAXVAL)
	{}

	~DiagonalStripPersistentDisplayListIds() {}

	void Initialize(World* world);
	void Release();

public:
	World* m_world;
	bool m_singleFrame;

	KyUInt32 m_displayListId_Left;
	KyUInt32 m_displayListId_Right;
	KyUInt32 m_displayListId_Corners;
	KyUInt32 m_displayListId_NearestOpposite;
	KyUInt32 m_displayListId_DiskToNearestOpposite;
	KyUInt32 m_displayListId_Diagonals;
	KyUInt32 m_displayListId_CornerIdx;
	KyUInt32 m_displayListId_Triangles;
};

class DiagonalStripDisplayConfig
{
public:
	DiagonalStripDisplayConfig() { SetDefaults(); }
	void SetDefaults();

	//DisplayListToggle m_toggleCorners;
	//DisplayListToggle m_toggleCornerIdx;
	//DisplayListToggle m_toggleCornerNearest;
	//DisplayListToggle m_toggleDiagonals;
	//DisplayListToggle m_toggleTriangles;

	KyFloat32 m_contourVerticalOffset;
	VisualShapeColor m_triangleColor;
	VisualShapeColor m_leftColor;
	VisualShapeColor m_rightColor;
	VisualShapeColor m_cornerDiskColor;
	VisualShapeColor m_diagonalColor;

	DiagonalStripPersistentDisplayListIds m_persistentDisplayListIds;
};


//////////////////////////////////////////////////////////////////////////
//
// StripCorner
//
//////////////////////////////////////////////////////////////////////////
enum CornerType
{
	UndefinedCornerType, // only when not initialized.
	PathStart,           // used only in string pulled path
	PathEnd,             // used only in string pulled path
	LeftSide,
	RightSide
};

class StripCorner
{
public:
	StripCorner()
		: m_cornerType(UndefinedCornerType)
		, m_maxRadius(0.0f)
	{}

	StripCorner(const Vec3f& position, CornerType cornerType, KyFloat32 maxRadius)
	{
		Set(position, cornerType, maxRadius);
	}

	static CornerType GetOppositeSide(CornerType cornerType)
	{
		switch (cornerType)
		{
		case LeftSide:              return RightSide;
		case RightSide:             return LeftSide;
		default:                    return cornerType;
		}
	}

	void Set(const Vec3f& position, CornerType cornerType, KyFloat32 maxRadius)
	{
		m_position = position;
		m_cornerType = cornerType;
		m_maxRadius = maxRadius;
		m_oppositeNearestPositionIsSet = false;
	}

	void SetCornerNearestOppositePosition(const Vec3f& nearestOppositePosition)
	{
		SetMaxRadius(0.49f * Distance2d(m_position, nearestOppositePosition));
		m_oppositeNearestPosition = nearestOppositePosition;
		m_oppositeNearestPositionIsSet = true;
	}

	void SetMaxRadius(KyFloat32 maxRadius)
	{
		m_maxRadius = maxRadius;
	}

	void PreciseType(CornerType cornerType) { m_cornerType = cornerType; }

	const Vec3f& GetPosition()   const { return m_position;   }
	CornerType   GetCornerType() const { return m_cornerType; }
	KyFloat32    GetMaxRadius()  const { return m_maxRadius;  }

	bool         IsOppositeNearestPositionSet() const { return m_oppositeNearestPositionIsSet; }
	const Vec3f& GetOppositeNearestPosition()   const { return m_oppositeNearestPosition;      }

	bool IsValid() const
	{
		return (m_cornerType != UndefinedCornerType);
	}


public:
	friend class DiagonalStrip;

	Vec3f m_position;
	CornerType m_cornerType;
	KyFloat32 m_maxRadius;

	Vec3f m_oppositeNearestPosition;
	bool m_oppositeNearestPositionIsSet;
};

KY_INLINE RotationDirection GetBubbleRotationDirectionFromCornerType(CornerType cornerType)
{
	return
		(cornerType == LeftSide) ? CounterClockwise :
		(cornerType == RightSide) ? Clockwise :
		UndefinedRotationDirection;
}


class NearestOppositePositionUpdator
{
public:
	NearestOppositePositionUpdator(const Vec3f& position)
		: m_position(position)
		, m_nearestPosition(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL)
		, m_minSqDist(KyFloat32MAXVAL)
	{}

	void UpdateWithOppositeSegment(const Vec3f& A, const Vec3f& B);

	Vec3f m_position;
	Vec3f m_nearestPosition;
	KyFloat32 m_minSqDist;
};

//////////////////////////////////////////////////////////////////////////
//
//  DiagonalStrip
//
//////////////////////////////////////////////////////////////////////////
class DiagonalStrip
{
public:
	enum GetBorderResult
	{
		BorderFound,
		BorderNotFound,
		NearestFoundIsStartOrEnd,
		AskedCornerIsDiagonalStripStartOrEnd
	};

	DiagonalStrip()
	{
	}

	void PushBackCorner(const Vec3f& position, CornerType cornerType, KyFloat32 maxRadius)
	{
		StripCorner corner(position, cornerType, maxRadius);
		m_corners.PushBack(corner);
	}

	void Clear();
	void SendVisualDebug(World* navWorld, DiagonalStripDisplayConfig& displayConfig);

	bool               IsEmpty()                     const { return m_corners.IsEmpty();  }
	KyUInt32           GetCornerCount()              const { return m_corners.GetCount(); }
	const StripCorner& GetCorner(KyUInt32 cornerIdx) const { return m_corners[cornerIdx]; }
	      StripCorner& GetCorner(KyUInt32 cornerIdx)       { return m_corners[cornerIdx]; }

	StripCorner GetIncomingBorderStart(KyUInt32 cornerIdx) const;
	StripCorner GetOutgoingBorderEnd(KyUInt32 cornerIdx) const;
	GetBorderResult GetNearestOppositeSidePosition(KyUInt32 cornerIdx, KyFloat32 searchRadius, Vec3f& nearestPosition) const;

	void SetCornerRadius(KyUInt32 cornerIdx, KyFloat32 cornerRadius)
	{
		KY_ASSERT(cornerIdx < m_corners.GetCount());
		m_corners[cornerIdx].m_maxRadius = cornerRadius;
	}

	void SetCornerNearestOppositePosition(KyUInt32 cornerIdx, const Vec3f& nearestOppositePosition)
	{
		KY_ASSERT(cornerIdx < m_corners.GetCount());
		m_corners[cornerIdx].SetCornerNearestOppositePosition(nearestOppositePosition);
	}

	void SetCornerMaxRadius(KyUInt32 cornerIdx, KyFloat32 maxRadius)
	{
		KY_ASSERT(cornerIdx < m_corners.GetCount());
		m_corners[cornerIdx].SetMaxRadius(maxRadius);
	}


private:
	void SendDiagonalStripVisualDebug(World* world, DiagonalStripDisplayConfig& displayConfig);
	void ApplyToVisibleOppositeSegmentsForward(NearestOppositePositionUpdator& updator, KyUInt32 cornerIdx, KyFloat32 searchRadius) const;
	void ApplyToVisibleOppositeSegmentsBackward(NearestOppositePositionUpdator& updator, KyUInt32 cornerIdx, KyFloat32 searchRadius) const;
public: // to call reserve
	KyArray<StripCorner> m_corners;

public:
	// dbg
	World* m_world;
};

} // namespace Kaim

#endif // Navigation_DiagonalStrip_H
