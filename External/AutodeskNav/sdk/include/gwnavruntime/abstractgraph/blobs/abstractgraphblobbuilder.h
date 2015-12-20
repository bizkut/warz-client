/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphBlobBuilder_H
#define Navigation_AbstractGraphBlobBuilder_H


#include "gwnavruntime/abstractgraph/blobs/abstractgraphblob.h"
#include "gwnavruntime/blob/baseblobbuilder.h"



namespace Kaim
{

class AbstractGraphGenerator;

class AbstractGraphBlobBuilder : public BaseBlobBuilder<AbstractGraphBlob>
{
public:
	AbstractGraphBlobBuilder(AbstractGraphGenerator* generator)
		: m_generator(generator)
	{}

private:
	virtual void DoBuild();

private:
	AbstractGraphGenerator* m_generator;
};

}

#endif
