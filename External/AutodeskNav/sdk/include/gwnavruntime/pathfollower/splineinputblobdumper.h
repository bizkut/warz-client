/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_CircleArcSplineComputerErrorCaseLogger_H
#define Navigation_CircleArcSplineComputerErrorCaseLogger_H


#include "gwnavruntime/base/types.h"


namespace Kaim
{

class CircleArcSplineComputer;

class SplineInputBlobDumper
{
public:
	SplineInputBlobDumper(CircleArcSplineComputer* computer, bool saveBlobAtDestruction, KyUInt32 maxSavedCase)
		: m_computer(computer)
		, m_maxSavedCases(maxSavedCase)
		, m_saveBlobAtDestruction(saveBlobAtDestruction)
	{}

	~SplineInputBlobDumper()
	{
		if (m_saveBlobAtDestruction)
			SaveBlob();
	}

	KyResult SaveBlob();

	CircleArcSplineComputer* m_computer;
	KyUInt32 m_maxSavedCases;
	bool m_saveBlobAtDestruction;
};

} // namespace Kaim

#endif // Navigation_CircleArcSplineComputerErrorCaseLogger_H
