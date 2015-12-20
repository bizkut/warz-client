/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_SingleStagePixel_H
#define GwNavGen_SingleStagePixel_H


#include "gwnavruntime/navmesh/navmeshtypes.h"


namespace Kaim
{


class SingleStagePixel
{
public:
	SingleStagePixel() { Clear(); }

	SingleStagePixel(KyFloat32 min_, KyFloat32 max_) :
		m_min(min_), m_max(max_)
	{}

	void Clear()
	{
		m_min = KyFloat32MAXVAL;
		m_max = -KyFloat32MAXVAL;
	}

	bool IsEmpty() const
	{
		return m_min == KyFloat32MAXVAL;
	}

	bool IsInside(KyFloat32 value) const
	{
		return value >= m_min && value <= m_max;
	}

	void AddValue(KyFloat32 value)
	{
		if (value < m_min) m_min = value;
		if (value > m_max) m_max = value;
	}

	void AddDownAndUp(KyFloat32 down, KyFloat32 up)
	{
		if (down < m_min) m_min = down;
		if (up > m_max) m_max = up;
	}

	void AddUp(KyFloat32 up)
	{
		if (up > m_max) m_max = up;
	}

	void AddDown(KyFloat32 down)
	{
		if (down < m_min) m_min = down;
	}

public:
	KyFloat32 m_min;
	KyFloat32 m_max;
};


}


#endif
