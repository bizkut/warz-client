//=========================================================================
//	Module: ParallelQuickSort.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "JobChief.h"

//////////////////////////////////////////////////////////////////////////

typedef int (*CompareFunc)(const void *, const void *);

//////////////////////////////////////////////////////////////////////////

class ParallelQSort
{
	struct SortRange
	{
		void *start;
		size_t size;
		SortRange(): start(0), size(0) {}
	};

	struct SortParams
	{
		SortRange *ranges;
		CompareFunc cmp;
		size_t elemntSize;
	};

	/** Maximum ranges that we can handle */
	static const int MAX_RANGES = 32;

	/**	Sort ranges that will be executed in parallel. */
	SortRange ranges[MAX_RANGES];

	/**	Current number of ranges. */
	size_t rangesMaxIdx;

	/**	Set minimum number of elements that considered as splitted. Should be approximately */
	size_t splittableRangeSize;

	/**	Byte-order aware memory copy function. Copy paste from crt qsort algorithm. */
	void SwapMemoryCRT(char *a, char *b, size_t w)
	{
		char tmp;
		if (a != b)
		{
			while (w--)
			{
				tmp = *a;
				*a++ = *b;
				*b++ = tmp;
			}
		}
	}

	/**	Memcpy version of memory swap. */
	void SwapMemoryMemCopy(char *a, char *b, size_t w)
	{
		if (a != b)
		{
			void *buf = alloca(w);
			memcpy(buf, a, w);
			memcpy(a, b, w);
			memcpy(b, buf, w);
		}
	}

	/**
	* Split range of values into sort chunks. Size of chunks controlled by splittableRangeSize var.
	* This function will return false if it is no enough memory for all splits, so we need fallback to ordinary sequential sort.
	*/
	bool SplitRange(void *start, size_t elementSize, size_t numOfElements, CompareFunc cmp)
	{
		if (numOfElements == 0) return true;
		//	Range too small to split or enough splits
		if (numOfElements < splittableRangeSize)
		{
			//	If we exceed number of space for splits, return false and fallback to sequential algorithm.
			if (rangesMaxIdx == MAX_RANGES) return false;
			ranges[rangesMaxIdx].start = start;
			ranges[rangesMaxIdx].size = numOfElements;
			++rangesMaxIdx;
			return true;
		}

		char *byteStart = reinterpret_cast<char*>(start);

		//	Slit range by choosing pivot element
		size_t pivotIndex = numOfElements - 1;
		void *pivot = byteStart + pivotIndex * elementSize;
		size_t storeIdx = 0;
		for (size_t i = 0; i < numOfElements - 1; ++i)
		{
			if (cmp(pivot, byteStart + i * elementSize) > 0)
			{
				char *a = byteStart + i * elementSize;
				char *b = byteStart + storeIdx * elementSize;
				SwapMemoryCRT(a, b, elementSize);
				++storeIdx;
			}
		}
		//	Place pivot at designated position
		SwapMemoryCRT(byteStart + storeIdx * elementSize, byteStart + pivotIndex * elementSize, elementSize);

		//	Now split 2 ranges
		bool rv = true;
		rv &= SplitRange(start, elementSize, storeIdx, cmp);
		rv &= SplitRange(byteStart + (storeIdx + 1) * elementSize, elementSize, numOfElements - storeIdx - 1, cmp);
		return rv;
	}

	/**	Worker function for JobChief. */
	static void SortWorker(void* data, size_t itemStart, size_t itemCount, size_t threadIndex )
	{
		(void)threadIndex;

		SortParams *sp = reinterpret_cast<SortParams*>(data);
		for (size_t i = itemStart; i < itemStart + itemCount; ++i)
		{
			SortRange &r = sp->ranges[i];
			qsort(r.start, r.size, sp->elemntSize, sp->cmp);
		}
	}

public:
	explicit ParallelQSort(size_t approxRangeSize)
	: splittableRangeSize(approxRangeSize)
	, rangesMaxIdx(0)
	{

	}

	/**	Execute sort algorithm. Function signature are the same as for CRT qsort call. */
	void Sort(void *data, size_t numOfElements, size_t sizeOfElement, CompareFunc cmp)
	{
		bool doParallel = SplitRange(data, sizeOfElement, numOfElements, cmp);
		if (doParallel)
		{
			SortParams sp;
			sp.ranges = ranges;
			sp.cmp = cmp;
			sp.elemntSize = sizeOfElement;

			g_pJobChief->Exec(&SortWorker, &sp, rangesMaxIdx);
		}
		else
		{
			qsort(data, numOfElements, sizeOfElement, cmp);
		}
	}
};