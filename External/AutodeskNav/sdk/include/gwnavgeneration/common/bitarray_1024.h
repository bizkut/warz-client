/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_BitArray_1024_H
#define GwNavGen_BitArray_1024_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/basesystem/logger.h"


namespace Kaim
{


class BitArray_1024
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	// KY_NULL as default value to avoid compilation error in KyArray instantiation
	// but it should already be used with a valid Heap
	BitArray_1024(MemoryHeap* heap = KY_NULL) : m_wordsCount(0), m_rootWord(0), m_words(KY_NULL), m_allocated(false), m_heap(heap)
	{}

	~BitArray_1024();

	KyResult Init(void* words, KyInt32 wordsCount);

	KyResult Init(KyInt32 wordsCount);

	void ClearBits();

	void SetBit(KyInt32 bit_idx)
	{
		KY_DEBUG_ASSERTN(bit_idx >= 0, ("bit_idx must be greater or equal to 0"));

		KyUInt32 word_idx = bit_idx >> 5; // x / 32
		KyUInt32 bit_idx_in_word = bit_idx & 0x1F; // (1 << 5) - 1

		KY_DEBUG_ASSERTN(word_idx < (KyUInt32)m_wordsCount, ("word_idx out of bounds"));

		m_rootWord |= (1 << word_idx);
		m_words[word_idx] |= (1 << bit_idx_in_word);
	}

	KyInt32 GetBit(KyInt32 bit_idx) const
	{
		KyUInt32 word_idx = bit_idx >> 5; // x / 32
		KyUInt32 bit_idx_in_word = bit_idx & 0x1F; // (1 << 5) - 1
		return m_words[word_idx] & (1 << bit_idx_in_word);
	}

	KyInt32 GetFirstBitIdx() const;

	KyInt32 GetLastBitIdx() const;

private:
	KyInt32 m_wordsCount;
	KyUInt32 m_rootWord;
	KyUInt32* m_words;
	bool m_allocated;
	MemoryHeap* m_heap;
};


}


#endif
