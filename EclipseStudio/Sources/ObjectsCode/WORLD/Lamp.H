
#ifndef __PWAR_OBJECT_LampBulb_H
#define __PWAR_OBJECT_LampBulb_H

#include "GameObjects\GameObj.h"
#include "TimeGradient.h"
#include "Tsg_stl/HashTable.h"

extern float __EditorLastLightRadius;

#define FNAME_LIGHT_PRESETS				"light_presets.ini"
#define UNUSED_PRESET_NAME				"Not used"
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class LightPreset
{
	struct LightPresetHashFunc_T
	{
		inline int operator () ( const char * szKey )
		{
			return r3dHash::MakeHash( szKey );// & ( LENGTH-1 );
		}
	};

	FixedString				m_sName;

	float					m_fRadiusScl;
	r3dTimeGradient2		m_tCurveRadius;
	r3dTimeGradient2		m_tCurveColorR;
	r3dTimeGradient2		m_tCurveColorG;
	r3dTimeGradient2		m_tCurveColorB;
	float					m_fTime;

	static HashTableDynamic< LightPreset *, FixedString, LightPresetHashFunc_T, 256	> m_dPresets;


public:
	static void				Clear				();

	float *					GetTimePtr			() 									{ return &m_fTime; }
	float *					GetRadiusSclPtr		() 									{ return &m_fRadiusScl; }
	r3dTimeGradient2 *		GetCurveRadiusPtr	() 									{ return &m_tCurveRadius; }
	r3dTimeGradient2 *		GetCurveColorRPtr	() 									{ return &m_tCurveColorR; }
	r3dTimeGradient2 *		GetCurveColorGPtr	() 									{ return &m_tCurveColorG; }
	r3dTimeGradient2 *		GetCurveColorBPtr	() 									{ return &m_tCurveColorB; }


	void					SetName				( const char * szName )				{ m_sName = szName; }
	void					SetTime				( float fTime )						{ m_fTime = fTime; }
	void					SetRadiusScl		( float fScl )						{ m_fRadiusScl = fScl; }
	void					SetCurveRadius		( const r3dTimeGradient2 &curve )	{ m_tCurveRadius = curve; }
	void					SetCurveColorR		( const r3dTimeGradient2 &curve )	{ m_tCurveColorR = curve; }
	void					SetCurveColorG		( const r3dTimeGradient2 &curve )	{ m_tCurveColorG = curve; }
	void					SetCurveColorB		( const r3dTimeGradient2 &curve )	{ m_tCurveColorB = curve; }

	const char *			GetName				()	const							{ return m_sName; }
	float					GetTime				()	const							{ return m_fTime; }
	float					GetRadiusScl		()	const							{ return m_fRadiusScl; }
	const r3dTimeGradient2 &GetCurveRadius		()	const							{ return m_tCurveRadius; }
	const r3dTimeGradient2 &GetCurveColorR		()	const							{ return m_tCurveColorR; }
	const r3dTimeGradient2 &GetCurveColorG		()	const							{ return m_tCurveColorG; }
	const r3dTimeGradient2 &GetCurveColorB		()	const							{ return m_tCurveColorB; }

	LightPreset()
	{
		Reset();
	}	

	LightPreset( const LightPreset &copy )
	{
		CopyFrom( copy );
	}	

	void CopyFrom( const LightPreset &copy )
	{
		m_sName = copy.m_sName;
		m_fTime = copy.m_fTime;
		m_fRadiusScl = copy.m_fRadiusScl;
		memcpy( &m_tCurveRadius, &copy.m_tCurveRadius, sizeof( m_tCurveRadius ) );
		memcpy( &m_tCurveColorR, &copy.m_tCurveColorR, sizeof( m_tCurveColorR ) );
		memcpy( &m_tCurveColorG, &copy.m_tCurveColorG, sizeof( m_tCurveColorG ) );
		memcpy( &m_tCurveColorB, &copy.m_tCurveColorB, sizeof( m_tCurveColorB ) );
	}	

	void Reset()
	{
		m_sName = UNUSED_PRESET_NAME;
		m_fTime = 0;
		m_fRadiusScl = 1.f;
		m_tCurveRadius.Reset();
		m_tCurveColorR.Reset();
		m_tCurveColorG.Reset();
		m_tCurveColorB.Reset();
	}	

	static void				AddPreset		( LightPreset * pPreset );
	static LightPreset *	FindPreset		( const char * szName );
	static void				FillNamesList	( stringlist_t &lst );
	static bool				SaveToScript	( const char * szFileName );
	static bool				LoadFromScript	( const char * szFileName );
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class obj_LightHelper : public GameObject
{
	DECLARE_CLASS(obj_LightHelper, GameObject)

private:

//	FixedString		m_sPreset;
	LightPreset *	m_pPreset;	
	
public:


	r3dColor	Color;
	r3dLight	LT;
	int 		bOn;
	float		innerRadius;
	float		outerRadius;

	int			bKilled;

	bool		bSerialized;


	float startTime;
	float endTime;
       	
public:

	void DoDraw();

	void SelectLightType() ;

#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor( float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected ) OVERRIDE;
#endif

	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();

	virtual	BOOL		Update();

	virtual GameObject * Clone ();

	virtual	BOOL		OnEvent(int event, void *data);

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);

	virtual BOOL		OnPositionChanged() OVERRIDE ;
	virtual	BOOL		OnOrientationChanged() OVERRIDE ;

	void				RecalcDirection() ;

	obj_LightHelper();
};

extern r3dLightSystem	WorldLightSystem;

#endif	// __PWAR_OBJECT_LampBulb_H
