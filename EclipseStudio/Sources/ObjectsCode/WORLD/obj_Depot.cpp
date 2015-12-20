#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "obj_Depot.h"


IMPLEMENT_CLASS(obj_Depot, "obj_Depot", "Object");
AUTOREGISTER_CLASS(obj_Depot);

static int __CMPID__ = 100;


//
//
//
//
//-----------------------------------------------------------------------
BOOL obj_Depot::Load(const char* fname)
//-----------------------------------------------------------------------
{
	parent::Load(fname);

  DrawOrder      = OBJ_DRAWORDER_FIRST;
  ObjFlags      |= OBJFLAG_SkipCastRay;
  MasterID	= __CMPID__++;


  Name = fname;
  
  return TRUE;
}


BOOL obj_Depot::OnCreate()
{
  parent::OnCreate();

/*	
  WIN32_FIND_DATA ffblk;
  HANDLE h;

  char Str[256];
  char Str1[256];


  sprintf (Str,"Data\\ObjectsDepot\\%s\\room.mat", Name.c_str());
  sprintf (Str1,"Data\\ObjectsDepot\\%s\\Textures\\", Name.c_str());
	
  r3dMaterialLibrary :: LoadLibrary(Str,Str1);

  sprintf(Str, "Data\\ObjectsDepot\\%s\\*.sco", Name.c_str());


  h = FindFirstFile(Str, &ffblk);
  if(h != INVALID_HANDLE_VALUE) {
    do {
	    sprintf(Str1, "Data\\ObjectsDepot\\%s\\%s", Name.c_str(), ffblk.cFileName);

		GameObject *obj = srv_CreateGameObject("obj_Building", Str1, r3dPoint3D(0,0,0), OBJ_CREATE_SKIP_POS);
		obj->CompoundID = MasterID;
		obj->CompoundOffset = obj->GetPosition();
		
    } while(FindNextFile(h, &ffblk) != 0);
    FindClose(h);
  }

  SetPosition(GetPosition());
*/  
  return TRUE;
}



BOOL obj_Depot::OnDestroy()
{
  return parent::OnDestroy();
}


BOOL obj_Depot::Update()
{
  return TRUE;
}

void  obj_Depot :: SetPosition(const r3dPoint3D& pos)
{
	GameObject::SetPosition(pos);
/*
	for(GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj)) 
	{
		if(!(obj->Class->Name == "obj_Building")) {
		  continue;
		}
	 
		if ( obj->CompoundID != MasterID) continue;

 		obj->SetPosition( GetPosition() + obj->CompoundOffset);
	}
*/	
}



void obj_Depot::ReadSerializedData(pugi::xml_node& node)
{
	GameObject::ReadSerializedData(node);
}

void obj_Depot::WriteSerializedData(pugi::xml_node& node)
{
	//GameObject::WriteSerializedData(node);
}

