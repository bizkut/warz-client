/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: NOBODY
#ifndef GwNavGen_InputProducerFromClientInput_H
#define GwNavGen_InputProducerFromClientInput_H

#include "gwnavruntime/base/memory.h"

namespace Kaim
{

class ClientInputConsumer;

class InputProducerFromClientInput
{
public:
	static KyResult Produce(const char* clientInputAbsoluteFileName, ClientInputConsumer& inputConsumer);
};

}


#endif // GwNavGen_PdgInputProducerFromClientInput_H

