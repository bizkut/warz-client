/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY

#ifndef Navigation_FloatStat_H
#define Navigation_FloatStat_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_String.h"


namespace Kaim
{

/// FloatStat maintains current, average, min, max statistics in a sliding window of frames.
/// The window size is 300 by default, this corresponds to 5 seconds at 60 FPS.
class FloatStat
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Statistics)

public:
	FloatStat() { ResetValues(); }
	FloatStat(const String& name, KyUInt32 windowSize = 300) { Init(name, windowSize); }

	void Init(const String& name, KyUInt32 windowSize = 300)
	{
		m_name = name;
		SetWindowSize(windowSize);
	}

	void SetWindowSize(KyUInt32 windowSize)
	{
		m_values.Resize(windowSize);
		ResetValues();
	}

	void ResetValues();
	void Update();

	const String& GetName()    const { return m_name; }
	KyFloat32     GetCurrent() const { return m_current; }
	KyFloat32     GetAverage() const { return m_average; }
	KyFloat32     GetMin()     const { return m_min; }
	KyFloat32     GetMax()     const { return m_max; }

	KyFloat32& GetWritableCurrent() { return m_current; }

	void SetCurrent(KyFloat32 current) { m_current = current; }

private:
	void AddValue();
	void ReplaceValue();
	void UpdateMin(KyUInt32 index);
	void UpdateMax(KyUInt32 index);

public:
	String m_name;
	KyFloat32 m_current;
	KyFloat32 m_average;
	KyFloat32 m_min;
	KyFloat32 m_max;

private:
	Array<KyFloat32> m_values; // values in the window
	KyUInt32 m_currentIndex;
	KyUInt32 m_minIndex;
	KyUInt32 m_maxIndex;
	bool m_cycled;
};

} // namespace Kaim

#endif // Navigation_FloatStat_H
