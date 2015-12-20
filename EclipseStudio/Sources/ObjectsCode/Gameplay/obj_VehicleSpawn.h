#pragma once

#include "GameCommon.h"
#include "BaseVehicleSpawnPoint.h"

#include "APIScaleformGfx.h"

class obj_VehicleSpawn : public BaseVehicleSpawnPoint
{
	DECLARE_CLASS(obj_VehicleSpawn, BaseVehicleSpawnPoint)
};
//#pragma once
//
//#include "GameCommon.h"
//
//class obj_Vehicle;
//// class to drop permanent weapon drops on server
//// client will ignore this object, it is only loaded on server!
//class obj_VehicleSpawn : public GameObject
//{
//	DECLARE_CLASS(obj_VehicleSpawn, GameObject)
//
//public:
//	obj_VehicleSpawn();
//	virtual ~obj_VehicleSpawn();
//
//	virtual	BOOL		OnCreate();
//	virtual	BOOL		Load(const char *name);
//
//	virtual	void		ReadSerializedData(pugi::xml_node& node);
//	virtual void		WriteSerializedData(pugi::xml_node& node);
//
//	virtual void		AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) OVERRIDE;
//	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;
//
//	virtual BOOL		Update();
//	void				DoDraw();
//
//#ifndef FINAL_BUILD
//	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
//
//#endif
//
//	void RespawnCar();
//
//	void clearVehicle();
//	void SetVehicleType( const r3dSTLString& preset );
//
//private:
//	char				vehicle_Model[64];
//	obj_Vehicle*		spawnedVehicle;
//
//};