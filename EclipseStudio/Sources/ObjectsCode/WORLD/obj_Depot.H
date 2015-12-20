#ifndef __GO_Depot_H
#define __GO_Depot_H

#include "obj_Depot.h"

class obj_Depot : public GameObject
{
	DECLARE_CLASS(obj_Depot, GameObject)

  public:
	  int			MasterID;

  public:
virtual	BOOL		Load(const char* name);
  
virtual	BOOL		OnCreate();
virtual	BOOL		OnDestroy();
virtual	BOOL		Update();

virtual	void 		SetPosition(const r3dPoint3D& pos);

virtual	void		ReadSerializedData(pugi::xml_node& node);
virtual void		WriteSerializedData(pugi::xml_node& node);
};


#endif	

