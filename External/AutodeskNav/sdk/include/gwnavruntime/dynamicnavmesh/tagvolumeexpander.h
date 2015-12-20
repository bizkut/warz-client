/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: LASI - secondary contact: NOBODY
#ifndef Navigation_TagVolumeExpander_H
#define Navigation_TagVolumeExpander_H

#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/math/vec2f.h"
#include "gwnavruntime/math/box3f.h"
#include "gwnavruntime/math/transform.h"

namespace Kaim
{

class TagVolume;

class TagVolumeCylinderExpander
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_TagVolume)
public:
	TagVolumeCylinderExpander(): 
		m_centerOfSymmetrySet(false), 
		m_centerOfSymmetry(Vec2f(KyFloat32MAXVAL, KyFloat32MAXVAL)) {}

public:
	void SetCenterOfSymmetry(const Vec2f& position) 
	{ 
		m_centerOfSymmetry = position; 
		m_centerOfSymmetrySet = true;
	}
	KyResult ComputeExpandedContour(const TagVolume* volume, KyFloat32 radius, KyArray<Vec2f>& expandedContour) const;
	bool IsCenterSet() const { return m_centerOfSymmetrySet; }
private:
	bool m_centerOfSymmetrySet;
	Vec2f m_centerOfSymmetry;
};

class TagVolumeBoxExpander
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_TagVolume)

public:
	void Initialize(const Transform& transform, const Box3f& localBox) 
	{ 
		m_localBox = localBox; 
		m_transform = transform;
	}
	KyResult ComputeExpandedContour(KyFloat32 radius, KyFloat32 mergePointDistance, KyArray<Vec2f>& expandedContour) const;
private:
	Box3f m_localBox;
	Transform m_transform;
};


} // Kaim

#endif // Navigation_TagVolumeTrigger_H
