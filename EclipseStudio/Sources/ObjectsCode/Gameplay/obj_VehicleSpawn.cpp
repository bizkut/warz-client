#include "r3dPCH.h"
#include "r3d.h"
//
//#include "Gameplay_Params.h"
//#include "GameCommon.h"
//#include "obj_VehicleSpawn.h"
//#include "../../Editors/ObjectManipulator3d.h"
//
//#include "../../GameEngine/gameobjects/obj_Vehicle.h"
//
//#include "gameobjects/obj_Mesh.h"
//
//#include "multiplayer/P2PMessages.h"
//#include "multiplayer/ClientGameLogic.h"
//
//#include "Editors/ObjectManipulator3d.h"
//
//#include "..\..\ui\HUDDisplay.h"
//extern HUDDisplay* hudMain;
//
//IMPLEMENT_CLASS(obj_VehicleSpawn, "obj_VehicleSpawn", "Object");
//AUTOREGISTER_CLASS(obj_VehicleSpawn);
//
//extern bool g_bEditMode;
//extern ObjectManipulator3d g_Manipulator3d;
//extern int CurHUDID;
//
//obj_VehicleSpawn::obj_VehicleSpawn()
//{
//	vehicle_Model[0] = '\0';
//	spawnedVehicle = NULL;
//
//	ObjTypeFlags |= OBJTYPE_VehicleSpawnPoint;
//}
//
//obj_VehicleSpawn::~obj_VehicleSpawn()
//{
//#ifdef VEHICLES_ENABLED
//	if( spawnedVehicle ) {
//		spawnedVehicle->setVehicleSpawner( NULL );
//		GameWorld().DeleteObject( spawnedVehicle );
//		spawnedVehicle = NULL;
//	} 
//#endif 
//}
//
//BOOL obj_VehicleSpawn::Load(const char *fname)
//{
//	const char* cpMeshName = "Data\\ObjectsDepot\\Capture_Points\\Flag_Pole_01.sco";
//
//	if(!parent::Load(cpMeshName))
//		return FALSE;
//
//#ifndef FINAL_BUILD
//	if (!g_bEditMode)
//#endif
//		ObjFlags |= OBJFLAG_SkipCastRay;
//
//#ifndef FINAL_BUILD
//	if (g_bEditMode)
//	{
//		setSkipOcclusionCheck(true);
//		ObjFlags |= OBJFLAG_AlwaysDraw | OBJFLAG_ForceSleep;
//	}
//#endif
//
//#ifdef VEHICLES_ENABLED
//	if(g_bEditMode)
//	{
//		RespawnCar();
//	}
//#endif
//
//	return TRUE;
//}
//
//BOOL obj_VehicleSpawn::OnCreate()
//{
//	parent::OnCreate();
//
//	DrawOrder	= OBJ_DRAWORDER_LAST;
//
//	r3dBoundBox bboxLocal ;
//
//	bboxLocal.Size = r3dPoint3D(5,3,10);
//	bboxLocal.Org = -bboxLocal.Size * 0.5f;
//	SetBBoxLocal( bboxLocal ) ;
//	UpdateTransform();
//
//	return 1;
//}
//
//BOOL obj_VehicleSpawn::OnDestroy()
//{
//	return parent::OnDestroy();
//}
//
//BOOL obj_VehicleSpawn::Update()
//{
//
//#ifndef FINAL_BUILD
//#ifdef VEHICLES_ENABLED
//	if ( g_bEditMode )
//	{
//		if( CurHUDID == 0 && d_drive_vehicles->GetBool() == false ) {
//			if ( spawnedVehicle != NULL && ( ( ( spawnedVehicle->GetPosition() - GetPosition()).LengthSq() > 1 && g_Manipulator3d.IsSelected( spawnedVehicle) == false )  ||  g_Manipulator3d.IsSelected( this ) ))
//			{
//				spawnedVehicle->SetPosition( GetPosition() );
//				spawnedVehicle->SetRotationVector( GetRotationVector() );
//			}
//		}
//	}
//#endif
//#endif 
//
//	return parent::Update();
//}
//
//void obj_VehicleSpawn::ReadSerializedData(pugi::xml_node& node)
//{
//	GameObject::ReadSerializedData(node);
//	pugi::xml_node myNode = node.child("VehicleSpawn");
//	_snprintf_s( vehicle_Model, 64, "%s", myNode.attribute("VehicleModel").value());
//	if ( g_bEditMode ) {
//		RespawnCar();
//	}
//}
//
//void obj_VehicleSpawn::WriteSerializedData(pugi::xml_node& node)
//{
//	GameObject::WriteSerializedData(node);
//	pugi::xml_node myNode = node.append_child();
//	myNode.set_name("VehicleSpawn");
//	myNode.append_attribute("VehicleModel") = vehicle_Model;
//}
//
//void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size);
//struct VehicleSpawnRenderableHelper : Renderable
//{
//	void Init()
//	{
//		DrawFunc = Draw;
//	}
//
//	static void Draw( Renderable* RThis, const r3dCamera& Cam )
//	{
//		VehicleSpawnRenderableHelper *This = static_cast<VehicleSpawnRenderableHelper*>( RThis );
//
//		r3dRenderer->SetTex(NULL);
//		r3dRenderer->SetMaterial(NULL);
//		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
//
//		This->Parent->DoDraw();
//	}
//
//	obj_VehicleSpawn* Parent;	
//};
//
//
//struct VehicleSpawnShadowGBufferRenderable : Renderable
//{
//	void Init()
//	{
//		DrawFunc = Draw;
//	}
//
//	static void Draw( Renderable* RThis, const r3dCamera& Cam )
//	{
//		VehicleSpawnShadowGBufferRenderable *This = static_cast<VehicleSpawnShadowGBufferRenderable*>( RThis );
//
//		
//	}
//
//	obj_VehicleSpawn*	Parent;
//	eRenderStageID		DrawState;
//};
//
//
//void obj_VehicleSpawn::DoDraw()
//{
//#ifndef FINAL_BUILD
//#ifdef VEHICLES_ENABLED
//	if( g_Manipulator3d.IsSelected(this) || ( spawnedVehicle != NULL && g_Manipulator3d.IsSelected(spawnedVehicle)) )
//#else
//	if( g_Manipulator3d.IsSelected(this) )
//#endif
//	{
//		r3dColor clr = r3dColor::blue;
//		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );
//
//		clr.A = 196;
//		r3dBoundBox localBBox = GetBBoxLocal();
//		r3dBoundBox bbox;
//		bbox.Org = GetPosition();// - 0.5f * m_bboxSize;
//		bbox.Org.y += .5f * localBBox.Size.y ; 
//		bbox.Size = localBBox.Size;
//
//		r3dVector rotation = GetRotationVector();
//		// there's a bug with rotation so I've to swap the axis. 
//		float temp;
//		temp = rotation.y;
//		rotation.y = rotation.x;
//		rotation.x = temp;
//
//		r3dDrawOrientedBoundBox( bbox, rotation, gCam, clr );
//
//		// r3dDrawUniformSphere ( GetPosition(), m_maxDist, gCam, clr );
//		r3dRenderer->Flush();
//	}
//#endif
//}
//
//#define RENDERABLE_OBJ_USER_SORT_VALUE (3*RENDERABLE_USER_SORT_VALUE)
//void obj_VehicleSpawn::AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) /*OVERRIDE*/
//{
//	if ( !g_bEditMode )
//		return;
//
//	VehicleSpawnShadowGBufferRenderable rend;
//
//	rend.Init();
//	rend.Parent		= this;
//	rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;
//	rend.DrawState	= rsCreateSM;
//
//	rarr.PushBack( rend );
//}
//
//void obj_VehicleSpawn::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
//{
//#ifdef FINAL_BUILD
//	return;
//#else
//	if ( !g_bEditMode )
//		return;
//
//	if( r_hide_icons->GetInt() )
//		return ;
//
//	float idd = r_icons_draw_distance->GetFloat();
//	idd *= idd;
//
//	if( ( Cam - GetPosition() ).LengthSq() > idd )
//		return;
//
//	// helper
//	extern int CurHUDID;
//	if(CurHUDID == 0)
//	{
//		VehicleSpawnRenderableHelper rend;
//		rend.Init();
//		rend.Parent		= this;
//		rend.SortValue	= RENDERABLE_OBJ_USER_SORT_VALUE;
//		render_arrays[ rsDrawComposite1 ].PushBack( rend );
//	}
//#endif
//}
//
//
//
//void obj_VehicleSpawn::SetVehicleType( const r3dSTLString& preset )
//{
//	_snprintf_s( vehicle_Model, 64, "%s", preset.c_str() );
//	RespawnCar();
//}
//
//void obj_VehicleSpawn::RespawnCar()
//{
//#ifdef VEHICLES_ENABLED
//	if( spawnedVehicle ) {
//		spawnedVehicle->setVehicleSpawner( NULL );
//		GameWorld().DeleteObject( spawnedVehicle );
//		spawnedVehicle = NULL;
//	} 
//
//	if( vehicle_Model[0] != '\0' ) {
//		spawnedVehicle = static_cast<obj_Vehicle*> ( srv_CreateGameObject("obj_Vehicle", vehicle_Model, GetPosition()));
//		spawnedVehicle->m_isSerializable = false;
//		spawnedVehicle->setVehicleSpawner( this );
//	}
//#endif
//}
//
//void obj_VehicleSpawn::clearVehicle()
//{
//	spawnedVehicle = NULL;
//}
//
//#ifndef FINAL_BUILD
//float obj_VehicleSpawn::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
//{
//	float starty = scry;
//
//	starty += parent::DrawPropertyEditor( scrx, scry, scrw, scrh, startClass, selected );
//
//	if( IsParentOrEqual( &ClassData, startClass ) )
//	{
//		starty += imgui_Static ( scrx, starty, "VehicleToSpawn" );
//
//		static stringlist_t vehicle_nameList;
//		static stringlist_t trueVehicleName;
//		uint32_t* vehicle_idList = 0;
//		if( vehicle_nameList.empty() )
//		{
//
//			vehicle_nameList.push_back( "None" );
//			trueVehicleName.push_back( "\0" );
//			FILE *f = fopen("Data/ObjectsDepot/Vehicles/DrivableVehiclesList.dat","rt");
//			while (!feof(f))
//			{
//				char name[64], fileName[64];
//				fscanf(f,"%s %s", name, fileName );
//				vehicle_nameList.push_back( name );
//				trueVehicleName.push_back( fileName );
//			}
//			fclose(f);
//		}
//
//		static const int width = 250;
//		static const int shift = 25;
//
//		static float primVehicleOffset = 0;
//		static int selectedVehicleIndex = 0;
//		// should we find the current index?
//		if( trueVehicleName[selectedVehicleIndex].compare(vehicle_Model) != 0 ) // restore selected item after loading level
//		{
//			for(uint32_t i=0; i<trueVehicleName.size(); ++i)
//			{
//				
//				if( trueVehicleName[i].compare(vehicle_Model) == 0 )
//				{
//					selectedVehicleIndex = i;
//					break;
//				}
//			}
//		}
//
//		int lastValue = selectedVehicleIndex;
//
//		imgui_DrawList(scrx, starty, (float)width, 150.0f, vehicle_nameList, &primVehicleOffset, &selectedVehicleIndex);
//		starty += 150;
//
//		if( lastValue != selectedVehicleIndex ) {
//
//			PropagateChange(trueVehicleName[selectedVehicleIndex], &obj_VehicleSpawn::SetVehicleType, selected ) ;
//		}
//	}
//
//	return starty-scry;
//}
//#endif
