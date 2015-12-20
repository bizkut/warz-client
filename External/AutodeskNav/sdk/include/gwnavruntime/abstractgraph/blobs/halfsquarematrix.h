/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_HalfSquaeMatrix_H
#define Navigation_HalfSquaeMatrix_H


#include "gwnavruntime/base/types.h"
#include "gwnavruntime/kernel/SF_Types.h"

namespace Kaim
{

template <class T>
class HalfSquareMatrix
{
public:
	HalfSquareMatrix()
		: m_buffer(KY_NULL)
		, m_bufferSize(0)
		, m_count(0)
	{}

	HalfSquareMatrix(T* buffer, KyUInt32 bufferSize, KyUInt32 count)
		: m_buffer(buffer)
		, m_bufferSize(bufferSize)
		, m_count(count)
	{}

	T* GetValue(KyUInt32 rowIdx, KyUInt32 columnIdx) const
	{
		if (rowIdx < columnIdx)
		{
			KyUInt32 index = GetIndexInHalfMatrix(rowIdx, columnIdx);
			return &m_buffer[index];
		}
		else if (rowIdx > columnIdx)
			return GetValue(columnIdx, rowIdx); // invert column and row
		else // if (rowIdx == columnIdx)
			return KY_NULL;
	}

public: //internal

	// rowIdx must be strictly smaller than columnIdx
	KyUInt32 GetIndexInHalfMatrix(KyUInt32 rowIdx, KyUInt32 columnIdx) const
	{
		KY_ASSERT(rowIdx < columnIdx);
		KyUInt32 count = GetCount();
		// index = (n*i+j) - (((i+1)^2)-(((i+1)^2)-(i+1))/2), where i is row, j is column, n is number of abstractGraphNodeCount
		KyUInt32 fullMatrixIdx = count * rowIdx + columnIdx; // n*i+j
		KyUInt32 rowSizedMatrixLength = rowIdx+1; // i+1
		KyUInt32 rowSizedMatrixSize = rowSizedMatrixLength*rowSizedMatrixLength; // (i+1)*(i+1)
		KyUInt32 rowSizedHalfMatrixSize = (rowSizedMatrixSize - rowSizedMatrixLength)/2; // (((i+1)*(i+1))-(i+1))/2
		KyUInt32 elementToRemoveFromFullMatrix = rowSizedMatrixSize - rowSizedHalfMatrixSize; // (((i+1)*(i+1))-(((i+1)*(i+1))-(i+1))/2)
		KyUInt32 index = fullMatrixIdx - elementToRemoveFromFullMatrix; // (n*i+j) - (((i+1)*(i+1))-(((i+1)*(i+1))-(i+1))/2)
		return index;
	}

	KyUInt32 GetCount() const
	{
		return m_count;
		// 		if (m_bufferSize == 0)
		// 			return 0;
		// 
		// 		// quadratic formula : (1)*n*n + (-1)*n + (-size*2) = 0
		// 		// discriminant : 1 - (4 * (-size*2)) //< always positive so there are 2 solutions
		// 		// n = (1 +/- sqrt(1-4*(-size*2)))/2*1 //< pickup only the positive solution
		// 		KyUInt32 res = (KyUInt32) (1.f + sqrtf(1.f+8.f*((KyFloat32)m_bufferSize)));
		// 		return (res >> 1);
	}

	KyUInt32 ComputeBufferSize(KyUInt32 count) const
	{
		return ((count*count)-count) / 2;
	}

public:
	// Here's how index of m_buffer are constructed,
	// values given into the array are the corresponding index in m_buffer
	// 
	// +-- Idx, m_count by m_count -----+
	// |                                |
	// V | 0 1 2 3 4   <----------------+
	// --+------------
	// 0 | # 0 1 2 3 T
	// 1 | x # 4 5 6 |
	// 2 | x x # 7 8 |
	// 3 | x x x # 9 L___ indices in m_buffer
	// 4 | x x x x #
	//
	// size of this array is (((n*n)-n) / 2) where n is m_count
	//
	T* m_buffer;
	KyUInt32 m_bufferSize;
	KyUInt32 m_count;
};

}

#endif
