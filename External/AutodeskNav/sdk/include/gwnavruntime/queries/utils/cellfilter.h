/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



#ifndef Navigation_CellFilter_H
#define Navigation_CellFilter_H

#include "gwnavruntime/database/databasegenmetrics.h"
#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/containers/bitfield.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/kernel/SF_Stats.h"
#include "gwnavruntime/kernel/SF_Memory.h"
#include "gwnavruntime/math/box2iiterator.h"
#include "gwnavruntime/base/types.h"

namespace Kaim
{


class CellFilter : public RefCountBaseNTS<CellFilter, Stat_Default_Mem>
{
	KY_CLASS_WITHOUT_COPY(CellFilter)

public:
	CellFilter();
	~CellFilter();

	void Clear();

	void ExpandCellBox(CellBox cellBox);
	
	void AddCellPos(const CellPos& cellPos);
	
	void AddAllPosInCellBox(CellBox cellBox);
	
	bool IsInFilter(const CellPos& cellPos) const;

	Ptr<CellFilter> Clone() const;
	
	void Display(ScopedDisplayList& displayList, const DatabaseGenMetrics& genMetrics) const;
	
public: //internal
	template <class BitFieldClass>
	void AddCellFilter(const CellBox& cellBox, const BitFieldClass& bitField);
	
	template <class BitFieldClass>
	static void Display(ScopedDisplayList& displayList, const DatabaseGenMetrics& genMetrics, const CellBox& cellBox, const BitFieldClass& cellBoxCoverage);

public:
	CellBox m_cellBox;
	BitField* m_cellBoxCoverage;
};

}

#include "gwnavruntime/queries/utils/cellfilter.inl"

#endif // Navigation_CellFilter_H