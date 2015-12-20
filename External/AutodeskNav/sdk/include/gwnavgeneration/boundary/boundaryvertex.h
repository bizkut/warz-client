/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_BoundaryVertex_H
#define GwNavGen_BoundaryVertex_H

#include "gwnavgeneration/boundary/boundarytypes.h"

namespace Kaim
{

class BoundaryEdge;

/*
Internal class to BoundaryGraph.
This class has no constructor for performance purpose. Be careful.
*/
class BoundaryVertex
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	void Init(const NavBoundaryPos& pos, KyFloat32 altitude, KyUInt32 index);

	void ComputeStaticStatus();

	bool IsStatic()              const;
	bool IsDiscontinuity()       const;
	bool IsNextToDiscontinuity() const;

	const ExclBoundaryPos& Pos() const;

	//needed in NavFloorBuilder (find())
	bool operator==(const BoundaryVertex& rhs) const;
	bool operator!=(const BoundaryVertex& rhs) const;

public:
	ExclBoundaryPos m_exclBoundaryPos; // relative to exclusivePixelBox
	KyFloat32 m_altitude;

	/*       |      
	//       3      
	//       \/     
	// --0-> V < 2--
	//       /\     
	//       1      
	//       |      
	*/
	BoundaryEdge* m_ins[4];

	/*       /\      
	//       1       
	//       |       
	// <-2---V---0-> 
	//       |       
	//       3       
	//       \/      
	*/
	BoundaryEdge* m_outs[4];

	BoundaryVertexStaticStatus m_staticStatus;
	KyUInt32 m_index;
};

KY_INLINE bool BoundaryVertex::IsStatic() const { return m_staticStatus == BoundaryVertexStaticStatus_Static /*|| IsDiscontinuity()*/; }
KY_INLINE bool BoundaryVertex::IsDiscontinuity() const { return m_staticStatus == BoundaryVertexStaticStatus_FeatureDiscontinuity; }
KY_INLINE const ExclBoundaryPos& BoundaryVertex::Pos() const { return m_exclBoundaryPos; }
KY_INLINE bool BoundaryVertex::operator!=(const BoundaryVertex& rhs) const { return !(*this == rhs); }
KY_INLINE bool BoundaryVertex::operator==(const BoundaryVertex& rhs) const
{
	return rhs.m_exclBoundaryPos == m_exclBoundaryPos && rhs.m_altitude == m_altitude && rhs.m_index == m_index;
}


} // namespace Kaim


#endif
