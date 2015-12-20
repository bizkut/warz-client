/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: JODA - secondary contact: GUAL
#ifndef GwNavGen_CombinaisonCalculator_H
#define GwNavGen_CombinaisonCalculator_H

#include "gwnavruntime/containers/kyarray.h"

namespace Kaim
{

class CombinaisonCalculator
{
public:
	CombinaisonCalculator(KyUInt32 maxElementCountInCombinaison)
	{
		m_maxBitIndex = maxElementCountInCombinaison - 1;
		m_maxBitMask = (1 << (m_maxBitIndex + 1)) - 1;
		m_bitMask = 1; // we dont consider the 
	}

	bool IsFinished() const
	{
		return m_bitMask > m_maxBitMask;
	}

	void Next()
	{
		++m_bitMask;
	}

	const KyArrayPOD<KyUInt32>& Compute()
	{
		m_currentCombinaison.Clear();

		for (KyUInt32 bitIndex = 0; bitIndex <= m_maxBitIndex; ++bitIndex)
		{
			KyUInt32 isBitAtIndex = (m_bitMask >> bitIndex) & 0x1;
			if (isBitAtIndex)
				m_currentCombinaison.PushBack(bitIndex);
		}

		return m_currentCombinaison;
	}

public:
	KyUInt32 m_maxBitIndex;
	KyUInt32 m_maxBitMask;
	KyUInt32 m_bitMask;
	KyArrayPOD<KyUInt32> m_currentCombinaison;
};


} // namespace Kaim


#endif
