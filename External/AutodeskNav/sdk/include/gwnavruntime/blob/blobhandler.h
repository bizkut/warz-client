/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobHandler_H
#define Navigation_BlobHandler_H


#include "gwnavruntime/blob/baseblobhandler.h"
#include "gwnavruntime/base/memory.h"


namespace Kaim
{

template<class T>
class BlobTypeDescriptor : public IBlobTypeDescriptor
{
public:
	virtual KyUInt32    GetBlobTypeId()      const { return T::GetBlobTypeId();      }
	virtual const char* GetBlobTypeName()    const { return T::GetBlobTypeName();    }
	virtual KyUInt32    GetBlobTypeVersion() const { return T::GetBlobTypeVersion(); }
	virtual void DoSwapEndianness(Endianness::Target e, void* blob) const { SwapEndianness(e, *((T*)blob)); }
	virtual IBlobTypeDescriptor* CreateBlobTypeDescriptor(int memStat = Stat_Default_Mem) const { return KY_NEW_ID(memStat) BlobTypeDescriptor<T>; }
	virtual BaseBlobHandler* CreateBlobHandler(int memStat = Stat_Default_Mem) const;
};

/// The BlobHandler class is a top-level mechanism for serializing blobs between objects in memory
/// and files on disk. It is responsible for maintaining an instance of its blob class, and allocating memory
/// for that object when needed through the Gameware Navigation memory management system.
/// \tparam T			The class of blob that this handler will manage.
/// This class transparently makes use of the BlobFileWriter and BlobFileReader classes to carry out the 
/// writing and reading of the data. If you want to use a different custom memory allocation scheme for your
/// blobs, you can invoke the BlobFileReader and BlobFileWriter classes directly. Mimic the inline implementations of
/// the BlobHandler class methods provided in the blobhandler.h file.
/// NOTE: the copy constructor and operator=() of this class transfer ownership over the blob memory to this object.
template<class T>
class BlobHandler : public BaseBlobHandler
{
	KY_CLASS_WITHOUT_COPY(BlobHandler)

public:
	BlobHandler() {}

	virtual KyUInt32    GetBlobTypeId()      const { return T::GetBlobTypeId();      }
	virtual const char* GetBlobTypeName()    const { return T::GetBlobTypeName();    }
	virtual KyUInt32    GetBlobTypeVersion() const { return T::GetBlobTypeVersion(); }
	virtual void DoSwapEndianness(Endianness::Target e, void* blob) const { SwapEndianness(e, *((T*)blob)); }
	virtual IBlobTypeDescriptor* CreateBlobTypeDescriptor(int memStat = Stat_Default_Mem) const { return KY_NEW_ID(memStat) BlobTypeDescriptor<T>; }
	virtual BaseBlobHandler*     CreateBlobHandler(       int memStat = Stat_Default_Mem) const { return KY_NEW_ID(memStat) BlobHandler<T>; }

	const T* Blob() const { return (const T*)m_blob; } /*< Retrieves the blob maintained by this handler. */
	T*       Blob()       { return (T*)m_blob; }       /*< Retrieves the blob maintained by this handler. */
};


template <class T>
BaseBlobHandler* BlobTypeDescriptor<T>::CreateBlobHandler(int memStat) const { return KY_NEW_ID(memStat) BlobHandler<T>; }

}


#endif

