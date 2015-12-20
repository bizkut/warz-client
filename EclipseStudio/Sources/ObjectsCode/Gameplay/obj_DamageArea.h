#pragma once

#include "GameCommon.h"

class obj_DamageArea : public GameObject
{
public:
	typedef r3dTL::TArray< obj_DamageArea* > Arr ;

private:
	DECLARE_CLASS(obj_DamageArea, GameObject)

	static Arr ms_DamageAreaArr ;

	float m_Radius;
	float m_Damage;
public:
	obj_DamageArea();
	virtual ~obj_DamageArea();

	float	GetRadius() const { return m_Radius; }

	virtual	BOOL		OnCreate();
	virtual	BOOL		Load(const char *name);

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam );

	virtual BOOL		Update();

#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE;
#endif

	R3D_FORCEINLINE static const Arr& GetDamageAreaArr() ;
};

R3D_FORCEINLINE /*static*/
const obj_DamageArea::Arr& obj_DamageArea::GetDamageAreaArr()
{
	return ms_DamageAreaArr ;
}