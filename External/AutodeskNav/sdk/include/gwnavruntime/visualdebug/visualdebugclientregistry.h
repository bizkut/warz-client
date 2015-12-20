/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: GUAL
#ifndef Navigation_MessageReceiverRegistry_H
#define Navigation_MessageReceiverRegistry_H

#include "gwnavruntime/blob/userdatablobregistry.h"

namespace Kaim
{

namespace Net
{
namespace AMP
{
class Message;
}
}


//Receiver interface in order to dispatch messages client side
/// IMessageReceiver defines an interface for objects that are called to process blobs of data sent through VisualDebug.
class IMessageReceiver
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)

public:
	virtual ~IMessageReceiver() {};

	/// This method is called to process each blob that has been successfully received.
	/// Your implementation should store message and ptrToBlob all together in order to keep ptrToBlob valid.
	/// \param message			The message received which maintains the memory that ptrToBlob points to.
	/// \param ptrToBlob		The received blob pointing into the memory of the message, it will remain valid as long as message is valid.
	/// \param blobSize			The size of the blob. 
	virtual void Receive(Ptr<Net::AMP::Message> message, const char* ptrToBlob, KyUInt32 blobSize) = 0;
	
};

typedef UserBlobRegistry<IMessageReceiver> MessageReceiverRegistry;

}

#endif // Navigation_MessageReceiverRegistry_H
