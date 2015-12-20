/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: JAPA - secondary contact: NOBODY
#ifndef GwNav_HeightFieldVisualGeometryBuilder_H
#define GwNav_HeightFieldVisualGeometryBuilder_H

#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

namespace Kaim
{

class HeightField;

class HeightFieldVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	HeightFieldVisualGeometryBuilder(HeightField* heightfield);
	~HeightFieldVisualGeometryBuilder();

	virtual void DoBuild();
private:
	Ptr<HeightField> m_heightfield;
};


}


#endif //GwNav_HeightFieldBlobVisualRepresentation_H
