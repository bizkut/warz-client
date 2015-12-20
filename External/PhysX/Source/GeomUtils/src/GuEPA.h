// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  


#ifndef PX_EPA_H
#define PX_EPA_H

#include "CmPhysXCommon.h"
#include "GuEPAFacet.h"
#include "PsAllocator.h"
#include "GuConvexSupportTable.h"
#include "GuGJKSimplex.h"
#include "GuGJKUtil.h"

namespace physx
{

#define MaxFacets 64
#define MaxSupportPoints 64
  
namespace Gu
{
	PxGJKStatus RecalculateSimplex(const ConvexV& a, const ConvexV& b, SupportMapPair* pair, PxI32* PX_RESTRICT aInd, PxI32* PX_RESTRICT bInd, PxU32 _size, Ps::aos::Vec3V& contactA, Ps::aos::Vec3V& contactB, Ps::aos::Vec3V& normal, Ps::aos::FloatV& penetrationDepth,
		const Ps::aos::BoolVArg aQuadratic, const Ps::aos::BoolVArg bQuadratic);

	class ConvexV;

	template<class Element, PxU32 Size>
	class BinaryHeap 
	{
	public:
		BinaryHeap() 
		{
			heapSize = 0;
		}
		
		~BinaryHeap() 
		{
		}
		
		PX_FORCE_INLINE Element* getTop() 
		{
			//return heapTop;//data[0];
			return data[0];
		}
		
		PX_FORCE_INLINE bool isEmpty()
		{
			return (heapSize == 0);
		}
		
		PX_FORCE_INLINE void makeEmpty()
		{
			heapSize = 0;
		}  

		PX_FORCE_INLINE void insert(Element* value)
		{
			PX_ASSERT((PxU32)heapSize < Size);
			PxU32 newIndex;
			PxI32 parentIndex = parent(heapSize);
			for (newIndex = heapSize; newIndex > 0 && (*data[parentIndex]) > (*value); newIndex = parentIndex, parentIndex= parent(newIndex)) 
			{
				data[ newIndex ] = data[parentIndex];
			}
			data[newIndex] = value; 
			heapSize++;
			PX_ASSERT(isValid());
		}


		PX_FORCE_INLINE Element* deleteTop() PX_RESTRICT
		{
			PX_ASSERT(heapSize > 0);
			PxI32 i, child;
			//try to avoid LHS
			PxI32 tempHs = heapSize-1;
			heapSize = tempHs;
			Element* PX_RESTRICT min = data[0];
			Element* PX_RESTRICT last = data[tempHs];
			PX_ASSERT(tempHs != -1);
			
			for (i = 0; (child = left(i)) < tempHs; i = child) 
			{
				/* Find smaller child */
				const PxI32 rightChild = child + 1;
				/*if((rightChild < heapSize) && (*data[rightChild]) < (*data[child]))
					child++;*/
				child += ((rightChild < tempHs) & ((*data[rightChild]) < (*data[child]))) ? 1 : 0;

				if((*data[child]) >= (*last))
					break;

				PX_ASSERT(i >= 0 && i < Size);
				data[i] = data[child];
			}
			PX_ASSERT(i >= 0 && i < Size);
			data[ i ] = last;
			/*heapTop = min;*/
			PX_ASSERT(isValid());
			return min;
		} 

		bool isValid()
		{
			Element* min = data[0];
			for(PxI32 i=1; i<heapSize; ++i)
			{
				if((*min) > (*data[i]))
					return false;
			}

			return true;
		}


		PxI32 heapSize;
//	private:
		Element* PX_RESTRICT data[Size];
		
		PX_FORCE_INLINE PxI32 left(PxI32 nodeIndex) 
		{
			return (nodeIndex << 1) + 1;
		}
		
		PX_FORCE_INLINE PxI32 parent(PxI32 nodeIndex) 
		{
			return (nodeIndex - 1) >> 1;
		}
	};

	class EPAFacetManager
	{
	public:
		EPAFacetManager() : currentID(0), freeIDCount(0), deferredFreeIDCount(0)	{}

		void	freeID(PxU8 id)
		{
			if(id == (currentID - 1))
				--currentID;
			else
			{
				PX_ASSERT(freeIDCount < MaxFacets);
				freeIDs[freeIDCount++] = id;
			}
		}

		void	deferFreeID(PxU8 id)
		{
			PX_ASSERT(deferredFreeIDCount < MaxFacets);
			deferredFreeIDs[deferredFreeIDCount++] = id;
		}

		void processDeferredIds()
		{
			for(PxU8 a = deferredFreeIDCount; a >0; --a)
			{
				freeID(deferredFreeIDs[a-1]);
			}
			deferredFreeIDCount = 0;;
		}

		void	freeAll()
		{
			currentID = 0;
			deferredFreeIDCount = freeIDCount = 0;
		}

		PxU8	getNewID()
		{
			// If recycled IDs are available, use them
			if(freeIDCount)
			{
				//const PxU8 size = --freeIDCount;
				const PxU8 size = freeIDCount-1;
				freeIDCount--;
				//const PxU8 id = freeIDs[size]; // Recycle last ID
				return freeIDs[size];// Recycle last ID
			}
			// Else create a new ID
			return currentID++;
		}

		PxU32 getNumFacets()
		{
			return currentID - freeIDCount;
		}
	private:
		PxU8				freeIDs[MaxFacets];
		PxU8				deferredFreeIDs[MaxFacets];
		PxU8				currentID;
		PxU8				freeIDCount;
		PxU8				deferredFreeIDCount;
	};

	class EPA
	{
	
	public:

		
		bool PenetrationDepth(const ConvexV& a, const ConvexV& b, SupportMapPair* pair, const Ps::aos::Vec3V* PX_RESTRICT Q, const Ps::aos::Vec3V* PX_RESTRICT A, const Ps::aos::Vec3V* PX_RESTRICT B, const PxI32 size, Ps::aos::Vec3V& pa, Ps::aos::Vec3V& pb);
		bool expandSegment(const ConvexV& a, const ConvexV& b, SupportMapPair* pair);
		bool expandTriangle(const ConvexV& a, const ConvexV& b, SupportMapPair* pair, PxI32& numVerts);
		
		bool addInitialFacet4();
		bool addInitialFacet5();
		bool expand(const Ps::aos::Vec3VArg q0, const Ps::aos::Vec3VArg q1, const Ps::aos::Vec3VArg q2, SupportMapPair* pair, PxI32& numVerts);
		Facet* addFacet(const PxU32 i0, const PxU32 i1, const PxU32 i2, const Ps::aos::FloatVArg lower2, const Ps::aos::FloatVArg upper2);
	
		bool originInTetrahedron(const Ps::aos::Vec3VArg p1, const Ps::aos::Vec3VArg p2, const Ps::aos::Vec3VArg p3, const Ps::aos::Vec3VArg p4);

		BinaryHeap<Facet, MaxFacets> heap;
		Ps::aos::Vec3V aBuf[MaxSupportPoints];
		Ps::aos::Vec3V bBuf[MaxSupportPoints];
		Facet facetBuf[MaxFacets];
		EdgeBuffer edgeBuffer;
		EPAFacetManager facetManager;
	};

	PX_FORCE_INLINE bool EPA::originInTetrahedron(const Ps::aos::Vec3VArg p1, const Ps::aos::Vec3VArg p2, const Ps::aos::Vec3VArg p3, const Ps::aos::Vec3VArg p4)
	{
		using namespace Ps::aos;
		const BoolV bFalse = BFFFF();
		return BAllEq(PointOutsideOfPlane4(p1, p2, p3, p4), bFalse) == 1;
	}

}

}

#endif
