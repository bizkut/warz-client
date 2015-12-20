/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: MAMU - secondary contact: LASI
#ifndef GwNavGen_ColDataIndexDescriptorBlobBuilder_FromGenerator_H
#define GwNavGen_ColDataIndexDescriptorBlobBuilder_FromGenerator_H


#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/collision/collisiondataindex.h"


namespace Kaim
{


class Generator;


class ColDataIndexBlobBuilder_FromGenerator : public BaseBlobBuilder<ColDataIndex>
{
public:
	ColDataIndexBlobBuilder_FromGenerator(Generator* generator) { m_generator = generator; }

private:
	virtual void DoBuild();

	Generator* m_generator;
};


}


#endif //GwNavGen_ColDataIndexDescriptorBlobBuilder_FromGenerator_H
