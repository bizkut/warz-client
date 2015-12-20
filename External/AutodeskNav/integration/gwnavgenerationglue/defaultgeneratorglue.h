/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



#ifndef KyGlue_DefaultGeneratorGlue_H
#define KyGlue_DefaultGeneratorGlue_H


#include "gwnavgeneration/generator/igeneratorglue.h"
#include "gwnavgenerationglue/defaultguidgeneratorinterface/defaultguidgeneratorinterface.h"
#include "gwnavgenerationglue/tbbparallelforinterface/tbbparallelforinterface.h"
#include "gwnavgenerationglue/tbbtlsalloc/tbbtlsalloc.h"
#include "gwnavruntimeglue/bulletcollisioninterface/bulletcollisioninterface.h"


namespace KyGlue
{

/// Default implementation of a class that hooks a third-party parallel processing framework to the
/// NavData generation system. This implementation uses the Threaded Building Blocks (TBB) library from Intel.
class DefaultGeneratorGlue : public Kaim::IGeneratorGlue
{
public:
	virtual Kaim::IParallelForInterface*   GetParallelFor()   { return &m_tbbparallelFor;       }
	virtual Kaim::ITlsAlloc*               GetTlsAlloc()      { return &m_tbbtlsAlloc;          }
	virtual Kaim::ICollisionInterface*    GetVisInterface()  { return &m_collisionInterface;   }

public:
	KyGlue::DefaultGuidGeneratorInterface   m_defaultGuidGenerator;
	KyGlue::TbbParallelForInterface         m_tbbparallelFor;
	KyGlue::TbbTlsAlloc                     m_tbbtlsAlloc;
	CollisionInterface                     m_collisionInterface;
};


}

#endif

