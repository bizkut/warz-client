/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_WorkingMemBitField_H
#define Navigation_WorkingMemBitField_H

#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"
#include "gwnavruntime/containers/bitfieldutils.h"

namespace Kaim
{

class WorkingMemBitField
{
public:
	WorkingMemBitField();

	WorkingMemBitField(WorkingMemory* workingMemory);

	void Init(WorkingMemory* workingMemory);

	// Check whether the container has been successfully initialized from a buffer of the WorkingMemory
	bool IsInitialized() const;

	KyUInt32 GetWordCount() const;
	KyUInt32 GetCapacity()  const;
	bool     IsEmpty()      const;

	void SetBit(KyUInt32 bitIndex);
	void UnsetBit(KyUInt32 bitIndex);
	bool IsBitSet(KyUInt32 bitIndex) const;

	void UnsetAllBits();
	void SetAllBits();


	KyResult SetMinimumBitCountAndUnsetAllBits(KyUInt32 bitCount) /* \pre IsInitialized() == true */
	{
		const KyUInt32 wordCount = BitFieldUtils::GetWordsCount(bitCount);
		KY_FORWARD_ERROR_NO_LOG(m_words.SetMinimumCapacity(wordCount));

		for(KyUInt32 i = m_words.GetCount(); i < wordCount; ++i)
			m_words.PushBack_UnSafe(0);

		return KY_SUCCESS;
	}

	bool IsFull() { return m_words.IsFull(); }                                    /* \pre IsInitialized() == true */
	void ReleaseWorkingMemoryBuffer() { m_words.ReleaseWorkingMemoryBuffer(); }
private:
	KyUInt32* GetWordsBuffer() { return m_words.GetBuffer(); }
	const KyUInt32* GetWordsBuffer() const { return m_words.GetBuffer(); }
public:
	WorkingMemArray<KyUInt32> m_words;
};

KY_INLINE WorkingMemBitField::WorkingMemBitField() {}
KY_INLINE WorkingMemBitField::WorkingMemBitField(WorkingMemory* workingMemory) : m_words(workingMemory) {}

KY_INLINE void WorkingMemBitField::Init(WorkingMemory* workingMemory) { m_words.Init(workingMemory); }

KY_INLINE bool WorkingMemBitField::IsInitialized()    const { return m_words.IsInitialized(); }

KY_INLINE KyUInt32 WorkingMemBitField::GetWordCount() const { return m_words.GetCount();    }
KY_INLINE KyUInt32 WorkingMemBitField::GetCapacity()  const { return m_words.GetCapacity(); }
KY_INLINE bool     WorkingMemBitField::IsEmpty()      const { return m_words.IsEmpty();     }

KY_INLINE void WorkingMemBitField::SetBit(KyUInt32 bitIndex)   { BitFieldUtils::SetBit(GetWordsBuffer(), bitIndex);          }
KY_INLINE void WorkingMemBitField::UnsetBit(KyUInt32 bitIndex) { BitFieldUtils::UnsetBit(GetWordsBuffer(), bitIndex);        }
KY_INLINE bool WorkingMemBitField::IsBitSet(KyUInt32 bitIndex)  const { return BitFieldUtils::IsBitSet(GetWordsBuffer(), bitIndex); }

KY_INLINE void WorkingMemBitField::UnsetAllBits() { memset(GetWordsBuffer(), '\0', GetWordCount() * sizeof(KyUInt32));   }
KY_INLINE void WorkingMemBitField::SetAllBits()   { memset(GetWordsBuffer(), '\xFF', GetWordCount() * sizeof(KyUInt32)); }

}



#endif

