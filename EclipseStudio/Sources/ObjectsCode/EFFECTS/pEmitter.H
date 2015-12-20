
#ifndef __PWAR_OBJECTS_PEmitter_H
#define __PWAR_OBJECTS_PEmitter_H

class obj_PEmitter : public obj_Building
{
	DECLARE_CLASS(obj_PEmitter, obj_Building)
  public:

  public:
virtual	BOOL		OnCreate();

virtual	BOOL		Update();

virtual	void		Draw(const r3dCamera& Cam, eRenderStageID DrawState);

};


#endif	// __PWAR_OBJECTS_PEmitter_H
