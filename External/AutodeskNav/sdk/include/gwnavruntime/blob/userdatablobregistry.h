/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_UserDataBlobRegistry_H
#define Navigation_UserDataBlobRegistry_H


#include "gwnavruntime/blob/blobregistry.h"
#include "gwnavruntime/kernel/SF_RefCount.h"


namespace Kaim
{


class UserDataBlobCategory
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	UserDataBlobCategory(BlobCategory* blobCategory);

	void SetUserData(KyUInt32 blobTypeId, void* userData)
	{
		KyUInt32 blobEnum = BlobTypeIdUtils::GetBlobEnum(blobTypeId);
		m_userDatas[blobEnum] = userData;
	}

	void* GetUserData(KyUInt32 blobTypeId) const
	{
		KyUInt32 blobEnum = BlobTypeIdUtils::GetBlobEnum(blobTypeId);
		return m_userDatas[blobEnum];
	}

public: // internal
	BlobCategory* m_blobCategory;
	KyArray<void*> m_userDatas;
};


class UserDataBlobNameSpace
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	UserDataBlobNameSpace(BlobNameSpace* blobNameSpace);
	~UserDataBlobNameSpace();
	KyResult SetUserData(KyUInt32 blobTypeId, void* userData);
	void* GetUserData(KyUInt32 blobTypeId) const;

public: // internal
	BlobNameSpace* m_blobNameSpace;
	KyArray<UserDataBlobCategory*> m_userDataCategories;
};


class UserDataBlobRegistry
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	~UserDataBlobRegistry();
	KyResult Set(KyUInt32 blobTypeId, void* userData);
	void* Get(KyUInt32 blobTypeId);

public: // internal
	UserDataBlobNameSpace* GetOrCreateUserDataNameSpace(BlobNameSpace* blobNameSpace);
	UserDataBlobNameSpace* GetUserDataNameSpace(BlobNameSpace* blobNameSpace);

	KyArray<UserDataBlobNameSpace*> m_userDataNameSpaces;
};

class UserBlobRegistryInternal
{
public: // internal
	static KyResult CheckCollisions(UserDataBlobRegistry& current, UserDataBlobRegistry& other);
	static void TransferContent(UserDataBlobRegistry& current, UserDataBlobRegistry& other);
	static void Clear(UserDataBlobRegistry& current);
};

class UserBlobRegistryIdx
{
public:
	UserBlobRegistryIdx()
		: m_nameSpaceIdx(0)
		, m_categoryIdx(0)
		, m_blobTypeEnum(0)
		, m_blobTypeId(KyUInt32MAXVAL)
	{}

	KyUInt32 GetBlobTypeId()
	{
		return m_blobTypeId;
	}

public: //internal
	void* GetAndAdvance(const UserDataBlobRegistry& registry);

	KyUInt32 m_nameSpaceIdx;
	KyUInt32 m_categoryIdx;
	KyUInt32 m_blobTypeEnum;

	KyUInt32 m_blobTypeId;
};


template<class T, class TOwned, bool IsTOwnedAutodelete>
class UserBlobRegistryBase
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	typedef UserBlobRegistryBase<T, TOwned, IsTOwnedAutodelete>  SelfType;

	~UserBlobRegistryBase()
	{
		Clear();
	}

	KyResult SetVal(KyUInt32 blobTypeId, T* object)
	{
		if (KY_FAILED(m_userDatas.Set(blobTypeId, object)))
			return KY_ERROR;
		m_owned.PushBack(object);
		return KY_SUCCESS;
	}

	KyResult SetRef(KyUInt32 blobTypeId, T* object)
	{
		return m_userDatas.Set(blobTypeId, object);
	}

	template<class BlobT>
	KyResult SetVal(T* object)
	{
		return SetVal(BlobT::GetBlobTypeId(), object);
	}

	template<class BlobT>
	KyResult SetRef(T* object)
	{
		return SetRef(BlobT::GetBlobTypeId(), object);
	}

	T* Get(KyUInt32 blobTypeId)
	{
		return (T*)m_userDatas.Get(blobTypeId);
	}

	/// Append the current registry with the content of the given registry (which loses ownership!)
	KyResult TransferContent(SelfType& fromRegistry)
	{
		// Check if this registry already contains elements from the given registry!
		KyResult result = UserBlobRegistryInternal::CheckCollisions(m_userDatas, fromRegistry.m_userDatas);
		if (KY_FAILED(result))
			return result;
		

		// If no collision, transfer the content,
		// First, transfer owned data
		m_owned.Reserve(m_owned.GetCount() + fromRegistry.m_owned.GetCount());
		m_owned.Append(fromRegistry.m_owned);
		fromRegistry.m_owned.Clear();

		// Second set userdata
		UserBlobRegistryInternal::TransferContent(m_userDatas, fromRegistry.m_userDatas);

		return KY_SUCCESS;
	}

	bool IsEmpty()
	{
		return m_userDatas.m_userDataNameSpaces.IsEmpty();
	}

	void Clear()
	{
		UserBlobRegistryInternal::Clear(m_userDatas);
		if (IsTOwnedAutodelete == false)
		{
			for (KyUInt32 i = 0; i < m_owned.GetCount(); ++i)
				delete m_owned[i];
		}
		m_owned.Clear();
	}

	T* GetAndAdvance(UserBlobRegistryIdx& idx) const
	{
		return (T*) idx.GetAndAdvance(m_userDatas);
	}


public:
	UserDataBlobRegistry m_userDatas;
	KyArray<TOwned> m_owned;
};

template<class T>
class UserBlobRegistry : public UserBlobRegistryBase<T, T*, false>
{

};

template<class T>
class UserPtrBlobRegistry : public UserBlobRegistryBase<T, Ptr<T>, true>
{

};


}


#endif

