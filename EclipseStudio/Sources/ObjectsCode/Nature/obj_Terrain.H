#ifndef __GO_TERRAIN_H
#define __GO_TERRAIN_H

#include "TrueNature2\Terrain3.h"
#include "TrueNature2\Terrain2.h"

class obj_Terrain : public GameObject
{
	DECLARE_CLASS(obj_Terrain, GameObject)

  public:

  public:
virtual	BOOL		Load(const char* name);
  
virtual	BOOL		OnCreate();
virtual	BOOL		OnDestroy();
virtual	BOOL		Update();

virtual	void		ReadSerializedData(pugi::xml_node& node);
virtual void		WriteSerializedData(pugi::xml_node& node);
};

extern obj_Terrain		*objTerrain;

#endif	

