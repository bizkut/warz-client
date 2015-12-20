
#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "Occluder.h"

//
//
// 	class for Occluder Object, as you may guess..
//
//


IMPLEMENT_CLASS(obj_Occluder, "obj_Occluder", "Object");
AUTOREGISTER_CLASS(obj_Occluder);

BOOL obj_Occluder::OnCreate()
{
 parent::OnCreate();

 ObjFlags |= OBJFLAG_CollideNever | OBJFLAG_SkipCastRay;

 //GameWorld.AddOccluder(MeshLOD[0]);

 return 1;
}



BOOL obj_Occluder::OnCollide(GameObject *tobj, CollisionInfo &trace)
{
 return TRUE;
}

int __bDraWOccluders = 0;


BOOL obj_Occluder::OnEvent(int event, void *data)
{
 if (event == OBJ_EVENT_GETEDITORCOMMANDS)
 {
  GOCommandArray *CA = (GOCommandArray *)data;
  
  CA->NumCommands = 2;
  CA->Commands[0].CommandID = OBJ_EVENT_USER + 100;
  strcpy(CA->Commands[0].Name,"Reinitialize");

  CA->Commands[1].CommandID = OBJ_EVENT_USER + 200;
  strcpy(CA->Commands[1].Name,"Draw ON/OFF");

  return TRUE;
 }

 switch (event)
 {
  case OBJ_EVENT_USER + 100:
//	GameWorld.InitOcclusionInfo();
	break;

  case OBJ_EVENT_USER + 200:
	__bDraWOccluders = 1 - __bDraWOccluders;
	break;

 };

 return FALSE;
}




BOOL obj_Occluder::Update()
{  
  return TRUE;
}


void obj_Occluder::Draw(const r3dCamera& Cam, eRenderStageID DrawState)
{
 if (DrawState != rsDrawComposite3) return;
 
 //if (__bDraWOccluders) pObject->DrawSimple(1);
}
