/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: MAMU - secondary contact: BRGR
#ifndef Navigation_IDisplayListBuilder_H
#define Navigation_IDisplayListBuilder_H

#include "gwnavruntime/blob/userdatablobregistry.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_String.h"

namespace Kaim
{

class ScopedDisplayList;

enum DisplayListBuilderParamType
{
	DisplayListBuilderParamType_Int,
	DisplayListBuilderParamType_Float,
	DisplayListBuilderParamType_Bool
};

class DisplayListBuilderParam
{
public:
	DisplayListBuilderParam()
		: m_name("Unset")
		, m_groupName("Unset")
		, m_type(DisplayListBuilderParamType_Bool)
		, m_intValue(0)
	{}

public:
	String m_name;
	String m_groupName;
	DisplayListBuilderParamType m_type;
	KyInt32 m_intValue; // Stores the integer and boolean value depending on the param type
	KyFloat32 m_floatValue;
};

// Key, type, value configuration
// NOTE:
// Create an enum to allow direct access to each value.
// In Add***Param, for each value, pass the corresponding enum to paramEnum.
// e.g.:
// - Write param => AddIntParam("Nearest bot index", 0, NEAREST_BOT_INDEX);
// - Read param  => KyInt32 myParam = config.GetIntParam(NEAREST_BOT_INDEX);
// 
class DisplayListBuilderConfig
{
public:
	void Init(KyUInt32 paramNumbers);
	void AddIntParam  (const char* name, KyInt32 defaultValue  , KyUInt32 paramEnum, const char* groupName = "");
	void AddFloatParam(const char* name, KyFloat32 defaultValue, KyUInt32 paramEnum, const char* groupName = "");
	void AddBoolParam (const char* name, bool defaultValue     , KyUInt32 paramEnum, const char* groupName = "");

	KyInt32   GetIntParam  (KyUInt32 paramEnum);
	KyFloat32 GetFloatParam(KyUInt32 paramEnum);
	bool      GetBoolParam (KyUInt32 paramEnum);

	void SetIntParam  (KyUInt32 paramEnum, KyInt32 value);
	void SetFloatParam(KyUInt32 paramEnum, KyFloat32 value);
	void SetBoolParam (KyUInt32 paramEnum, bool value);

public:
	KyArray<DisplayListBuilderParam> m_params;
};

class BlobData
{
public:
	BlobData(char* blob, KyUInt32 deepBlobSize, KyUInt32 blobTypeId)
		: m_blob(blob), m_deepBlobSize(deepBlobSize), m_blobTypeId(blobTypeId) {}

	char* m_blob;
	KyUInt32 m_deepBlobSize;
	KyUInt32 m_blobTypeId;
};

// Interface to help to fulfill a displayList from a blob
class IDisplayListBuilder
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)
public:
	IDisplayListBuilder() : m_doBuild(true) {}
	virtual ~IDisplayListBuilder() {}

public:
	// Setup your display list builder configuration here (see m_config)
	virtual void Init() {}
	
	// Helper to get other blob from blobTypeId
	static const void* GetBlob(KyArray<BlobData>& blobDataArray, KyUInt32 blobTypeId);

	void Build(ScopedDisplayList* displayList, char* blob, KyUInt32 deepBlobSize)
	{
		if (m_doBuild)
			DoBuild(displayList, blob, deepBlobSize);
	}

private:
	// It is the caller's responsability to ensure that the given void* is really pointing to a blob of the blob type expected by the implementation!
	// Recommendation is to associate the instance of IDisplayListBuilder to the blob type thanks to UserBlobRegistry!
	// You can setup a configuration in the Init function, and use it during DoBuild in order to allow the user to setup
	// the way the display list is built.
	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 deepBlobSize = 0) = 0;

public:
	bool m_doBuild;
	DisplayListBuilderConfig m_config;

protected:
	bool IsVisualDebugSupported(ScopedDisplayList*);
	bool IsRendererSupported(ScopedDisplayList*);
};

class DisplayListBuilder : public IDisplayListBuilder
{
public:
	virtual void DoBuild(ScopedDisplayList* displayList, char* blob, KyUInt32 deepBlobSize = 0);
};

// Registry that associates blobs to IDisplayListBuilder
// Intended to be used to fulfill the displayList client-side (typically this is done by the NavigationLab)
typedef UserBlobRegistry<IDisplayListBuilder> DisplayListBuilderBlobRegistry;
void AddVisualDebugDisplayListBuildersToRegistry(DisplayListBuilderBlobRegistry& registry);
void AddQueriesDisplayListBuildersToRegistry(DisplayListBuilderBlobRegistry& registry);

}

#endif // Navigation_DisplayList_H
