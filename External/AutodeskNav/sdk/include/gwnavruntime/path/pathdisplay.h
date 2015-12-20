/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: GUAL
#ifndef Navigation_PathVisRep_H
#define Navigation_PathVisRep_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/visualsystem/visualcolor.h"

namespace Kaim
{

class ScopedDisplayList;
class PathBlob;

enum DrawingMode
{
	OneColor,
	TwoColor,
	ThreeColors
};

class PathDisplayListBuilder
{
public:
	PathDisplayListBuilder()
	{
		SetDefaults();
	}

	void SetDefaults()
	{
		m_altitudeOffset = 0.0f;
		m_drawingMode = OneColor;

		m_nodesColor = Kaim::VisualColor::Cyan;
		m_nodesAltitudeOffset = 0.5f;
		m_nodesHalfWidth = 0.1f;

		m_edgesColor = Kaim::VisualColor::LawnGreen;
		m_edgesAltitudeOffset = 0.5f;
		m_secondEdgesColor = Kaim::VisualColor::Cyan;
		m_thirdEdgesColor = Kaim::VisualColor::Cyan;
		m_firstPathSplit = KY_NULL;
		m_secondPathSplit = KY_NULL;
	}

	void DisplayPath(Kaim::ScopedDisplayList* displayList, const Kaim::PathBlob* pathBlob, bool displayChannel = true);

public:
	KyFloat32 m_altitudeOffset;
	DrawingMode m_drawingMode;
	Kaim::VisualColor m_edgesColor;
	Kaim::VisualColor m_nodesColor;
	KyFloat32 m_nodesAltitudeOffset;
	KyFloat32 m_nodesHalfWidth;
	KyFloat32 m_edgesAltitudeOffset;
	Kaim::VisualColor m_secondEdgesColor;
	Kaim::VisualColor m_thirdEdgesColor;
	const Kaim::Vec3f* m_firstPathSplit;
	const Kaim::Vec3f* m_secondPathSplit;
};

} // namespace Kaim

#endif // Navigation_PathVisRep_H
