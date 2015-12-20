/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


#ifndef GwNavGen_AbstractDataIndexBlobBuilder_FromGenerator_H
#define GwNavGen_AbstractDataIndexBlobBuilder_FromGenerator_H


#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/abstractgraph/blobs/abstractdataindex.h"


namespace Kaim
{


class Generator;


class AbstractDataIndexBlobBuilder_FromGenerator : public BaseBlobBuilder<AbstractDataIndex>
{
public:
	AbstractDataIndexBlobBuilder_FromGenerator(Generator* generator) { m_generator = generator; }

private:
	virtual void DoBuild();

	Generator* m_generator;
};


}


#endif
