/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_BitFieldBlob_H
#define Navigation_BitFieldBlob_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/containers/bitfieldutils.h"
#include "gwnavruntime/blob/baseblobbuilder.h"

namespace Kaim
{

class BitFieldBlob
{
public:
	BitFieldBlob() {}

	KY_INLINE KyUInt32 GetBitCount() const { return m_bitsCount; }
	KY_INLINE bool IsBitSet(KyUInt32 bitIndex) const { return BitFieldUtils::IsBitSet(m_words.GetValues() , bitIndex); }
public:
	KyUInt32 m_bitsCount;
	BlobArray<KyUInt32> m_words;
};

inline void SwapEndianness(Endianness::Target e, BitFieldBlob& self)
{
	SwapEndianness(e, self.m_bitsCount);
	SwapEndianness(e, self.m_words);
}

class BitFieldBase;
class BitFieldBlobBuilder : public BaseBlobBuilder<BitFieldBlob>
{
public:
	BitFieldBlobBuilder(BitFieldBase* bitField) : m_bitField(bitField) {}

	virtual void DoBuild();
	
public:
	BitFieldBase* m_bitField;
};

} // Kaim

#endif // Navigation_BitFieldBlob_H
