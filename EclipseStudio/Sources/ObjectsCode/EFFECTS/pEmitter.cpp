#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"

#include "PEmitter.h"
#include "PT_GeneralEmmiter.h"


IMPLEMENT_CLASS(obj_PEmitter, "obj_PEmitter", "Object");
AUTOREGISTER_CLASS(obj_PEmitter);


//
//
//
//
BOOL obj_PEmitter::OnCreate()
{
 parent::OnCreate();

 DrawOrder	= OBJ_DRAWORDER_LAST;
  char  buf[MAX_PATH];
  char afname[MAX_PATH];

  
  sprintf(buf, "Data\\Particles\\Particles.cfg");
  r3dFile *f = r3d_open(buf, "rt");
  if(!f) { return FALSE; }

  while(!feof(f)) 
  {
    if(fgets(buf, sizeof(buf), f) == NULL) break;
    if(*buf == ';') continue;
    if(strlen(buf) < 3) continue;
    
    char pAnimName[MAX_PATH];
    char pAnimFile[MAX_PATH];
    sscanf(buf, "%s %s", pAnimName, pAnimFile);

    
    sprintf(afname, "Data\\Particles\\%s", pAnimFile);

    if(strstr(Name.c_str(),pAnimName))
    {
     obj_GeneralParticleEmmiter* obj = (obj_GeneralParticleEmmiter*)srv_CreateGameObject("obj_GeneralParticleEmmiter", afname, MeshLOD[0]->CentralPoint);
     obj->ObjFlags &= ~OBJFLAG_SkipCastRay; //NOTE: level-created particle emitters should be selectable

//     obj->propPlace.sectName = Name.c_str();

     r3dOutToLog("EMMITER   %s\n", afname);
    }
  }
  fclose(f);


  return TRUE;
}


void obj_PEmitter::Draw( const  r3dCamera &Cam, eRenderStageID DrawState)
{
 return ; //parent::Draw(Cam, DrawState);
}



BOOL obj_PEmitter::Update()
{
 return FALSE;
}


