/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_GamewareKey_H
#define Navigation_GamewareKey_H


#include "gwnavruntime/base/types.h"
#include "gwnavruntime/kernel/SF_String.h"


namespace Kaim
{

class File;
class FileOpenerBase;
class GamewareKey;

class GamewareKeyConfig
{
public:
	GamewareKeyConfig()
		: m_gamewareKey(KY_NULL)
		, m_gamewareKeyFileName(KY_NULL)
		, m_fileOpener(KY_NULL)
	{}

	//void InitWithGamewareKey(const char* gamewareKey)
	//{
	//	m_gamewareKey = gamewareKey;
	//	m_gamewareKeyFileName = KY_NULL;
	//	m_fileOpener = KY_NULL;
	//}

	//void InitWithGamewareKeyFileName(const char* gamewareKeyFileName, FileOpenerBase* fileOpener = KY_NULL)
	//{
	//	m_gamewareKey = gamewareKey;
	//	m_gamewareKeyFileName = KY_NULL;
	//	m_fileOpener = fileOpener;
	//}

public:
	const char* m_gamewareKey;
	const char* m_gamewareKeyFileName;
	FileOpenerBase* m_fileOpener;
};


class GamewareKey
{
public:
	static GamewareKey& GetInstance()
	{
		static GamewareKey s_instance;
		return s_instance;
	}

	KyResult Init(const GamewareKeyConfig& config);

	const char* GetKey() { return m_key; }

private:
	GamewareKey() { m_key[0] = '\0'; }
	KyResult SetKeyFromFile(const char* fileName, FileOpenerBase* fileOpener);
	KyResult SetKeyFromFile(File* file);
	void SetKey(const char* key);

private:
	char m_key[128];
};

}

#endif
