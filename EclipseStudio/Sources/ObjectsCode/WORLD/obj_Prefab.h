//=========================================================================
//	Module: obj_Prefab.h
//	Copyright (C) 2013.
//=========================================================================

#pragma once
#include "../../../GameEngine/gameobjects/GameObj.h"
#include "PrefabManager.h"

//////////////////////////////////////////////////////////////////////////

class obj_Prefab: public GameObject
{
	friend class CreatePrefabUndo;

	DECLARE_CLASS(obj_Prefab, GameObject)

	r3dSTLString prefabName;
	GameObjects gameObjects;
	bool isSelected;
	float drawDistance;
	
	typedef r3dTL::TArray<D3DXMATRIX> MatrixArray;
	MatrixArray localPoses;

	void DeleteObjects();
	
	bool isOpened;
	void DrawBoundsWhenOpened();

	bool recalcChildPoses;
	void UpdateChildPoses(D3DXMATRIX localPos);

public:
	obj_Prefab();
	~obj_Prefab();

	r3dBoundBox CalcBoundingBox();

	virtual	void ReadSerializedData(pugi::xml_node& node);
	virtual void WriteSerializedData(pugi::xml_node& node);
	void Instantiate(const r3dSTLString &name, const pugi::xml_node &description);
	void CreateXMLDescription(pugi::xml_node &rootNode);
	void SetPreviewMode(bool isPreview);
	void SelectPrefab(bool ignoreOpenState = false, bool forceSelectDirectParent = false);
	virtual void OnPickerDrop();
	void BreakSelf();

	uint32_t GetNumObjects() const;
	void GetGameObjects(GameObjects &outObjs) const;
	const r3dVector & GetObjectWorldPos(uint32_t idx) const;
	r3dVector GetObjectLocalPos(uint32_t idx) const;

	virtual void SetPosition(const r3dPoint3D& pos);
	virtual void SetRotationVector(const r3dVector& Angles);
	virtual BOOL OnDestroy();
	virtual BOOL Update();
	virtual void SetObjFlags(int objFlags);
	virtual void ResetObjFlags(int objFlags);

	virtual int IsStatic() { return true; }

	virtual GameObject * Clone();
	void NotifyObjectDestruction(GameObject *go);

	bool IsInternalObjectsValid() const;
	void CleanInvalidObjects();

	void OpenForModification();
	void CloseAfterModification();

	bool IsOpenedForEdit() const;
	void AddNewObject(GameObject *objToAdd);
	void RemoveObject(GameObject *objToRemove);

	bool IsPartOfPrefab(GameObject *objToVerify) const;

#ifndef FINAL_BUILD
#ifndef WO_SERVER
	virtual	float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected);
#endif
#endif
};
