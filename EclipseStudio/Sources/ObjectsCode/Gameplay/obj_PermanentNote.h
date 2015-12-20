#pragma once

#include "GameCommon.h"
#include "GameCode/UserProfile.h"
#include "obj_Note.h"

class obj_PermanentNote : public obj_Note
{
	DECLARE_CLASS(obj_PermanentNote, obj_Note)
private:
	char m_msgIDString[128];

public:
	obj_PermanentNote();
	virtual ~obj_PermanentNote();

	virtual	BOOL		OnCreate();

#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif
	virtual void		WriteSerializedData(pugi::xml_node& node);
	virtual	void		ReadSerializedData(pugi::xml_node& node);
};
