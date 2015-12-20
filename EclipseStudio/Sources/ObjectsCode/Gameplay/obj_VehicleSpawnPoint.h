#pragma once

#include "GameCommon.h"
#include "BaseItemSpawnPoint.h"
#include "APIScaleformGfx.h"

class obj_VehicleSpawnPoint : public BaseItemSpawnPoint
{
	DECLARE_CLASS(obj_VehicleSpawnPoint, BaseItemSpawnPoint)

protected:
	int selectedSpawnPoint;
	bool isEditorCheckSpawnPointAtStart;
	r3dColor editorSpawnPointBoxColor;

	int maxSpawns;

	r3dgameVector(uint32_t) VehicleSpawnSelection;

public:
	obj_VehicleSpawnPoint();
	virtual ~obj_VehicleSpawnPoint();

	virtual BOOL Load(const char *fname);
	virtual BOOL OnCreate();
	virtual BOOL OnDestroy();
	virtual BOOL Update();

#ifndef FINAL_BUILD
	virtual float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	void DoDrawComposite(const r3dCamera& camera);

	virtual void AppendShadowRenderables(RenderArray& renderArray, int sliceIndex, const r3dCamera& camera);
	virtual void AppendRenderables(RenderArray(&renderArrays)[rsCount], const r3dCamera& camera) OVERRIDE;

	virtual BOOL OnNetReceive(DWORD eventID, const void* packetData, int packetSize);
	void SetSelectedSpawnPoint(int ssp) { selectedSpawnPoint = ssp; }

	virtual void OnPickerPick(int locationId);
	virtual GameObject *Clone();

	virtual void WriteSerializedData(pugi::xml_node& node);
	virtual void ReadSerializedData(pugi::xml_node& node);
};