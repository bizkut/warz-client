/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobFile_H
#define Navigation_BlobFile_H


#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/blob/blobfileheader.h"
#include "gwnavruntime/kernel/SF_File.h"
#include "gwnavruntime/blob/blobhandler.h"

namespace Kaim
{

class File;
class FileOpenerBase;


/// The BlobFile class reads/writes the data maintained in a blob from/to a file.
class BlobFile
{
public:
	static KyResult Save(File* file, const BaseBlobHandler& blobHandler, Endianness::Type endianness = Endianness::BigEndian);

	static KyResult Save(const char* fileName, FileOpenerBase* fileOpener, const BaseBlobHandler& blobHandler, Endianness::Type endianness = Endianness::BigEndian);

	static void* LoadVoid(const char* fileName, FileOpenerBase* fileOpener, BaseBlobHandler& blobHandler);

	static void* LoadVoid(File* file, BaseBlobHandler& blobHandler);

	template<class T>
	static T* Load(const char* fileName, FileOpenerBase* fileOpener, BlobHandler<T>& blobHandler)
	{
		return (T*)LoadVoid(fileName, fileOpener, blobHandler);
	}

	template<class T>
	static T* Load(File* file, BlobHandler<T>& blobHandler)
	{
		return (T*)LoadVoid(file, blobHandler);
	}

private:
	static KyResult WriteBuffers(File* file, void* fileHeaderMemory, void* blobMemory, KyUInt32 blobSize);
	static void CloseFile(File* fileToClose);
};

} // namespace Kaim


#endif
