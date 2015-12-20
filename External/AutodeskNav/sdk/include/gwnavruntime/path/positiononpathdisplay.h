/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_PositionOnPathVisRep_H
#define Navigation_PositionOnPathVisRep_H

#include "gwnavruntime/visualsystem/idisplaylistbuilder.h"
#include "gwnavruntime/visualsystem/visualcolor.h"

namespace Kaim
{

class PositionOnPath;
class PositionOnPathBlob;

class PositionOnPathDisplayListBuilder : public IDisplayListBuilder
{
public:
	PositionOnPathDisplayListBuilder()
		: m_positionColor(VisualColor::Magenta)
		, m_pointHalfWidth(0.1f)
		, m_pointHeight(2.5f)
		, m_circleSubdivision(5)
	{}

	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 deepBlobSize = 0);
	//virtual void Push(ScopedDisplayList* displayList, const PositionOnPath* posOnPath);

public:
	VisualColor m_positionColor;
	KyFloat32 m_pointHalfWidth;
	KyFloat32 m_pointHeight;
	KyUInt32 m_circleSubdivision;
};

} // namespace Kaim

#endif // Navigation_PositionOnPathVisRep_H
