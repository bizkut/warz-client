
#ifndef __PWAR_OBJECT_OCCLUDER_H
#define __PWAR_OBJECT_OCCLUDER_H

class obj_Occluder : public obj_Building
{
	DECLARE_CLASS(obj_Occluder, obj_Building)
  public:

  public:

virtual	BOOL		OnCreate();
virtual	BOOL		OnCollide(GameObject *tobj, CollisionInfo &trace);
virtual	BOOL		Update();
virtual	BOOL		OnEvent(int event, void *data);
virtual	void		Draw(const r3dCamera& Cam, eRenderStageID DrawState);

};

#endif	// __PWAR_OBJECT_OCCLUDER_H
