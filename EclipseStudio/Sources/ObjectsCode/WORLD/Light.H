
#ifndef __PWAR_OBJECT_LampBulb_H
#define __PWAR_OBJECT_LampBulb_H

#include "building.h"

class obj_LampBulb : public obj_Building
{
	DECLARE_CLASS(obj_LampBulb, obj_Building)
  public:
	r3dColor	Color;
	r3dLight	LT;
	int 		bOn;
	int		bHaveRays;
	int		bHaveLight;
	int		bCastShadows;
	int		bAffectLightmap;
	int		bStatic;

	int		EditMode;
	float		FadeVal;

	r3dBoundBox	LightBBox;
 IDirect3DQuery9* 	Query;
	int		VisiblePixels;
	

       	
  public:
virtual	BOOL		OnCreate();
virtual	BOOL		OnDestroy();

virtual	BOOL		Update();
virtual	BOOL		OnCollide(GameObject *tobj, CollisionInfo &trace);

virtual	BOOL		OnEvent(int event, void *data);

virtual	void		Draw(r3dCamera& Cam, int DrawState = OBJ_DRAW_DEFAULT);


	void		EditColor(int event);
	void		EditGeneral(int event);
};


#endif	// __PWAR_OBJECT_LampBulb_H
