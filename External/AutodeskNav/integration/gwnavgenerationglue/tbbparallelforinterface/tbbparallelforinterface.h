/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



#ifndef KyGlue_TbbParallelForInterface_H
#define KyGlue_TbbParallelForInterface_H


#include "gwnavgeneration/generator/iparallelforinterface.h"


namespace KyGlue
{

class TbbParallelForInterface_Implementation;

/// This implementation of Kaim::IParallelForInterface uses the open-source Threaded Building Blocks (TBB) libraries from Intel
/// to parallelize NavData generation across all available processors on the computer.
/// You can use this class directly, or use its source code as a model for developing your own implementations
/// of Kaim::IParallelForInterface to support third-party parallel processing systems.
class TbbParallelForInterface : public Kaim::IParallelForInterface
{
public:
	TbbParallelForInterface();
	virtual ~TbbParallelForInterface();
	virtual KyResult Init();
	virtual KyResult Terminate();
	virtual KyResult ParallelFor(void** elements, KyUInt32 elementsCount, Kaim::IParallelElementFunctor* elementFunctor);

private:
	TbbParallelForInterface_Implementation* m_impl;
};


}


#endif
