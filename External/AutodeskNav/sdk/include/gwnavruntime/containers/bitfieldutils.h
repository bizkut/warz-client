/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_BitFieldUtils_H
#define Navigation_BitFieldUtils_H

#include "gwnavruntime/base/types.h"

namespace Kaim
{

class BitFieldUtils
{
public:
	static KY_INLINE KyUInt32 GetWordIndex(KyUInt32 bitIndex)            { return bitIndex >> 5;        } // / 32
	static KY_INLINE KyUInt32 GetBitIndexInWord(KyUInt32 bitIndex)       { return bitIndex & 0x1F;      } // % 32
	static KY_INLINE KyUInt32 GetWordsCount(KyUInt32 bitCount)           { return (bitCount + 31) >> 5; } // / 32
	static KY_INLINE KyUInt32 GetWordMaskFromBitIndex(KyUInt32 bitIndex) { return GetWordMask(GetBitIndexInWord(bitIndex)); }

	static KY_INLINE void SetBit(KyUInt32& word, KyUInt32 bitIndex)   { word |= GetWordMaskFromBitIndex(bitIndex);              }
	static KY_INLINE void UnsetBit(KyUInt32& word, KyUInt32 bitIndex) { word &= ~GetWordMaskFromBitIndex(bitIndex);             }
	static KY_INLINE bool IsBitSet(KyUInt32 word, KyUInt32 bitIndex)  { return (word & GetWordMaskFromBitIndex(bitIndex)) != 0; }

	static KY_INLINE void SetBit(KyUInt32* words, KyUInt32 bitIndex)         { SetBit(words[GetWordIndex(bitIndex)], bitIndex);          }
	static KY_INLINE void UnsetBit(KyUInt32* words, KyUInt32 bitIndex)       { UnsetBit(words[GetWordIndex(bitIndex)], bitIndex);        }
	static KY_INLINE bool IsBitSet(const KyUInt32* words, KyUInt32 bitIndex) { return IsBitSet(words[GetWordIndex(bitIndex)], bitIndex); }

	static KY_INLINE KyUInt32 GetWordMask(KyUInt32 bitIndexInWord)
	{
		// note that "1U << bitIndexInWord" may  generate microcoded instructions and impacts performances
		// as on powerPC (see http://cellperformance.beyond3d.com/articles/2006/04/avoiding-microcoded-instructions-on-the-ppu.html)
		static const KyUInt32 s_maskToIsolateBit[32] =
		{
			       1, 1U <<  1, 1U <<  2, 1U <<  3, 1U <<  4, 1U <<  5, 1U <<  6, 1U <<  7,
			1U <<  8, 1U <<  9, 1U << 10, 1U << 11, 1U << 12, 1U << 13, 1U << 14, 1U << 15,
			1U << 16, 1U << 17, 1U << 18, 1U << 19, 1U << 20, 1U << 21, 1U << 22, 1U << 23,
			1U << 24, 1U << 25, 1U << 26, 1U << 27, 1U << 28, 1U << 29, 1U << 30, 1U << 31
		};

		return s_maskToIsolateBit[bitIndexInWord];
	}
};


}


#endif // Navigation_BitFieldUtils_H

