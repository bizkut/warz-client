/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// Primary contact: LASI - secondary contact: GUAL
#ifndef GwNavGen_NavMeshElementBlobBuilder_H
#define GwNavGen_NavMeshElementBlobBuilder_H


#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/navmesh/blobs/navmeshelementblob.h"
#include "gwnavruntime/containers/kyarray.h"


namespace Kaim
{

class GeneratorSystem;
class DynamicNavCell;
class GeneratorGuidCompound;
class NavCellBlob;


class NavMeshElementBlobBuilder : public BaseBlobBuilder<NavMeshElementBlob>
{
public:
	NavMeshElementBlobBuilder(
		GeneratorSystem* sys, const GeneratorGuidCompound* guidCompound, const KyArray< Ptr<BlobHandler<NavCellBlob> > >* navCellHandlers,
		NavMeshElementBlob::NavMeshFlags flags = NavMeshElementBlob::NAVMESH_NO_FLAG);

private:
	virtual void DoBuild();
	void ComputeAndSetCellBox();
private:
	GeneratorSystem* m_sys;
	const GeneratorGuidCompound* m_guidCompound;
	const KyArray< Ptr<BlobHandler<NavCellBlob> > >* m_navCellHandlers;
	NavMeshElementBlob::NavMeshFlags m_navmeshFlags;
};

}


#endif
