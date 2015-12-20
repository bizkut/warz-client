/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: JAPA - secondary contact: NOBODY
#ifndef GwNav_IndexedMeshVisualGeometryBuilder_H
#define GwNav_IndexedMeshVisualGeometryBuilder_H

#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

namespace Kaim
{

class BlobAggregate;

class IndexedMeshVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	IndexedMeshVisualGeometryBuilder(BlobAggregate* aggregrate);
	~IndexedMeshVisualGeometryBuilder();
	virtual void DoBuild();

private:
	Ptr<BlobAggregate> m_aggregrate;
};


}


#endif //GwNav_IndexedMeshBlobVisualRepresentation_H
