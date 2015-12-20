/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphFloorBlobBuilder_H
#define Navigation_AbstractGraphFloorBlobBuilder_H


#include "gwnavruntime/abstractgraph/blobs/abstractgraphfloorblob.h"
#include "gwnavruntime/blob/baseblobbuilder.h"


namespace Kaim
{

class AbstractGraphFloorGenerator;

class AbstractGraphFloorBlobBuilder : public BaseBlobBuilder<AbstractGraphFloorBlob>
{
public:
	AbstractGraphFloorBlobBuilder(AbstractGraphFloorGenerator* generator)
		: m_generator(generator)
	{}

private:
	virtual void DoBuild();

private:
	AbstractGraphFloorGenerator* m_generator;
};


}


#endif
