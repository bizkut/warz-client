/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_BitField_H
#define Navigation_BitField_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/containers/bitfieldutils.h"

namespace Kaim
{

class BitFieldBlob;

class BitFieldBase
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:

	BitFieldBase() : m_words(KY_NULL), m_wordsCapacity(0), m_bitsCount(0) {}
	~BitFieldBase() { KY_FREE(m_words); }

	KyUInt32 GetCount() const;

	void SetBit(KyUInt32 bitIndex);
	void UnsetBit(KyUInt32 bitIndex);
	bool IsBitSet(KyUInt32 bitIndex) const;

	void UnsetAllBits();
	void SetAllBits();

	// bitValue must be 0 or 1
	void SetBitValue(KyUInt32 bitIndex, KyUInt32 bitValue);
	void SetBitValue(KyUInt32 bitIndex, bool bitValue);

	// return 0 or 1
	KyUInt32 GetBitValue(KyUInt32 bitIndex) const;

	void Clear(); // frees memory !

protected:
	void Resize_(KyUInt32 newBitsCount, int memStat, MemoryHeap* heap = KY_NULL);
	void Reserve_(KyUInt32 newBitsCount, int memStat, MemoryHeap* heap = KY_NULL);
	void InitFromBlob_(const BitFieldBlob& bitFieldBlob, int memStat, MemoryHeap* heap = KY_NULL);
private:
	KyUInt32  Word(KyUInt32 bitIndex) const;
	KyUInt32& Word(KyUInt32 bitIndex)      ;

private:
	friend class BitFieldBlob;
	friend class BitFieldBlobBuilder;

	KyUInt32* m_words;
	KyUInt32 m_wordsCapacity;
	KyUInt32 m_bitsCount;
};


template<int SID>
class BitFieldMemStat : public BitFieldBase
{
	KY_DEFINE_NEW_DELETE_OPERATORS(SID)
public:
	KY_INLINE explicit BitFieldMemStat() : BitFieldBase() {}
	KY_INLINE explicit BitFieldMemStat(KyUInt32 newBitsCount)     {       Resize_(newBitsCount, SID); }
	KY_INLINE void Resize(KyUInt32 newBitsCount)                  {       Resize_(newBitsCount, SID); }
	KY_INLINE void Reserve(KyUInt32 newBitsCount)                 {      Reserve_(newBitsCount, SID); }
	KY_INLINE void InitFromBlob(const BitFieldBlob& bitFieldBlob) { InitFromBlob_(bitFieldBlob, SID); }
};

typedef BitFieldMemStat<Stat_Default_Mem> BitField;



KY_INLINE void     BitFieldBase::SetBitValue(KyUInt32 bitIndex, KyUInt32 bitValue) { bitValue == 0 ? UnsetBit(bitIndex) : SetBit(bitIndex); }
KY_INLINE void     BitFieldBase::SetBitValue(KyUInt32 bitIndex, bool bitValue) { bitValue == false ? UnsetBit(bitIndex) : SetBit(bitIndex); }
KY_INLINE KyUInt32 BitFieldBase::GetBitValue(KyUInt32 bitIndex) const { return (Word(bitIndex) >> BitFieldUtils::GetBitIndexInWord(bitIndex)) & 0x00000001; }

KY_INLINE void     BitFieldBase::SetBit(KyUInt32 bitIndex)            { BitFieldUtils::SetBit(m_words, bitIndex); }
KY_INLINE void     BitFieldBase::UnsetBit(KyUInt32 bitIndex)          { BitFieldUtils::UnsetBit(m_words, bitIndex); }
KY_INLINE bool     BitFieldBase::IsBitSet(KyUInt32 bitIndex)    const { return BitFieldUtils::IsBitSet(m_words , bitIndex); }
KY_INLINE KyUInt32 BitFieldBase::GetCount()                     const { return m_bitsCount; }

KY_INLINE KyUInt32  BitFieldBase::Word(KyUInt32 bitIndex) const { return m_words[BitFieldUtils::GetWordIndex(bitIndex)]; }
KY_INLINE KyUInt32& BitFieldBase::Word(KyUInt32 bitIndex)       { return m_words[BitFieldUtils::GetWordIndex(bitIndex)]; }

} // Kaim

#endif // Navigation_BitField_H
