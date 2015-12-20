#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "Lamp.h"

#include "GameLevel.h"

#include "Editors/ObjectManipulator3d.h"
#include "ui/UIimEdit2.h"


//
//
// 	class for LampBulb Object, as you may guess..
//
//


float __EditorLastLightRadius = 3.0f;

IMPLEMENT_CLASS(obj_LightHelper, "obj_LightHelper", "Object");
AUTOREGISTER_CLASS(obj_LightHelper);


extern r3dLightSystem	WorldLightSystem;

extern	bool g_bEditMode;
extern int g_bForceQualitySelectionInEditor;
static r3dTexture *LampIcon = NULL;

//--------------------------------------------------------------------------------------------------------
obj_LightHelper::obj_LightHelper() 
	: m_pPreset( NULL )
	, innerRadius(0)
	, outerRadius(__EditorLastLightRadius)
	, startTime(0.0f)
	, endTime(24.0f)
{ 
	bOn = false;
	bKilled = false;
	Color = r3dColor(255,100,30); 
	bSerialized = false; 
}

BOOL obj_LightHelper::OnCreate()
{
 parent::OnCreate();

 DrawOrder	= OBJ_DRAWORDER_LAST;

 bKilled = 0;

#ifndef FINAL_BUILD
 if(g_bEditMode)
	r_show_light_helper->SetInt( true );
 else
	r_show_light_helper->SetInt( false );

 if(!g_bEditMode)
#endif
 {
	 ObjFlags |= OBJFLAG_SkipCastRay;
 }

 ObjFlags |= OBJFLAG_DisableShadows;
 

 if (!LampIcon) LampIcon = r3dRenderer->LoadTexture("Data\\Images\\Lamp.dds");

 bOn		= 1;

 LT.Assign(GetPosition().X,GetPosition().Y,GetPosition().Z);
 LT.SetRadius (innerRadius, outerRadius);
 LT.SetColor(Color.R, Color.G, Color.B);

 SelectLightType();

 if ( LT.Type == R3D_SPOT_LIGHT )
 {
   if( !bSerialized )
   {
	   LT.SpotAngleOuter = 45.0f;
	   LT.SpotAngleInner = 33.0f;
	   LT.SpotAngleFalloffPow = 1.0f;
	   SetRotationVector(r3dVector(0, 90, 0));
   }
 }
 else
 if( LT.Type == R3D_TUBE_LIGHT )
 {
	 if( !bSerialized )
	 {
		 LT.SetWidthLength( 0.f, 1.f );
	 }
 }
 else
 if( LT.Type == R3D_PLANE_LIGHT )
 {
	 if( !bSerialized )
	 {
		 LT.SetWidthLength( 1.f, 1.f );
	 }
 }

 if( !bSerialized )
 {
	 LT.Intensity = 1.f ;
 }


 WorldLightSystem.Add(&LT); 


 r3dBoundBox bboxLocal ;

 if( g_bEditMode )
 {
  bboxLocal.Size = r3dPoint3D(2,2,2);
  bboxLocal.Org = -bboxLocal.Size * 0.5f;
 }
 else
 {
  bboxLocal.Size = r3dPoint3D( 0.f, 0.f, 0.f );
  bboxLocal.Org	= -r3dPoint3D( .0f, .0f, .0f );
 }

 SetBBoxLocal( bboxLocal );

 // prevent scaling cause it's not obvious with light object yet can distort scene box significantly
 SetScale( r3dPoint3D( 1.f, 1.f, 1.f ) );

 GameObject::UpdateTransform();

 return 1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	 
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------------
void LightPreset::Clear() 
{
	m_dPresets.IterateStart();

	while ( m_dPresets.IterateNext() )
	{
		LightPreset * pPreset = m_dPresets.IterateGet();
		PURE_DELETE( pPreset );
	} 	

	m_dPresets.Clear();
}

//--------------------------------------------------------------------------------------------------------
void LightPreset::AddPreset( LightPreset * pPreset )
{ 
	m_dPresets.Add( pPreset->m_sName, pPreset );
}

//--------------------------------------------------------------------------------------------------------
LightPreset * LightPreset::FindPreset( const char * szName )		
{ 
	return m_dPresets.GetObject( szName ); 
}


//--------------------------------------------------------------------------------------------------------
void LightPreset::FillNamesList( stringlist_t &lst )
{
	m_dPresets.IterateStart();

	while ( m_dPresets.IterateNext() )
	{
		const LightPreset * pPreset = m_dPresets.IterateGet();
		lst.push_back( pPreset->m_sName.c_str() );
	} 	
}

//--------------------------------------------------------------------------------------------------------
void WriteCurve( FILE * hFile, const r3dTimeGradient2 &curve, const char * szName )
{ 
	fprintf( hFile, "\t%s\n\t{\n", szName );
	for( int i = 0; i < curve.NumValues; i ++ )
	{
		fprintf( hFile, "\t\tval: %f %f\n",  curve.Values[ i ].time,  curve.Values[ i ].val[ 0 ] );
	}
	fprintf( hFile, "\t}\n\n" );
}

//--------------------------------------------------------------------------------------------------------
void LoadCurve( Script_c &script, r3dTimeGradient2 &curve )
{ 
	char buffer[ MAX_PATH ];

	script.SkipToken( "{" );
	while ( ! script.EndOfFile() )
	{
		script.GetToken( buffer );
		if ( *buffer == '}' )
			break;

		if ( ! strcmp( buffer, "val:" ) )
		{
			float fTime = script.GetFloat();
			float fVal = script.GetFloat();
			curve.AddValue( fTime, fVal );
		}
	}
}

//--------------------------------------------------------------------------------------------------------
bool LightPreset::SaveToScript( const char * szFileName )
{ 
	FILE * hFile = fopen( szFileName, "wt" );
	if ( ! hFile )
		return false;

	m_dPresets.IterateStart();

	while ( m_dPresets.IterateNext() )
	{
		const LightPreset * pPreset = m_dPresets.IterateGet();
		
		fprintf( hFile, "preset: \"%s\"\n{\n", pPreset->m_sName.c_str() );
		fprintf( hFile, "\ttime: %f\n", pPreset->m_fTime );
		fprintf( hFile, "\tradius_scl: %f\n", pPreset->m_fRadiusScl );
		WriteCurve( hFile, pPreset->m_tCurveRadius, "radius" );
		WriteCurve( hFile, pPreset->m_tCurveColorR, "color_r" );
		WriteCurve( hFile, pPreset->m_tCurveColorG, "color_g" );
		WriteCurve( hFile, pPreset->m_tCurveColorB, "color_b" );

		fprintf( hFile, "}\n\n" );
	} 		

	fclose( hFile );
	return true;
}

//--------------------------------------------------------------------------------------------------------
bool LightPreset::LoadFromScript( const char * szFileName )
{ 
	Script_c script;

	if ( ! script.OpenFile( szFileName ) )
		return false;

	Clear();

	char buffer[ MAX_PATH ];

	while ( ! script.EndOfFile() )
	{
		script.GetToken( buffer );
		if ( ! *buffer )
			continue;

		if ( ! strcmp( buffer, "preset:" ) )
		{
			LightPreset * pPreset = gfx_new LightPreset;

			script.GetString( buffer, sizeof( buffer ) );
			pPreset->SetName( buffer );
			script.SkipToken( "{" );

			while ( ! script.EndOfFile() )
			{
				script.GetToken( buffer );
				if ( *buffer == '}' )
					break;

				if ( ! strcmp( buffer, "time:" ) )
				{
					pPreset->SetTime( script.GetFloat() );
				}
				else if ( ! strcmp( buffer, "radius_scl:" ) )
				{
					pPreset->SetRadiusScl( script.GetFloat() );
				}
				else if ( ! strcmp( buffer, "radius" ) )
				{
					LoadCurve( script, pPreset->m_tCurveRadius );
				}
				else if ( ! strcmp( buffer, "color_r" ) )
				{
					LoadCurve( script, pPreset->m_tCurveColorR );
				}
				else if ( ! strcmp( buffer, "color_g" ) )
				{
					LoadCurve( script, pPreset->m_tCurveColorG );
				}
				else if ( ! strcmp( buffer, "color_b" ) )
				{
					LoadCurve( script, pPreset->m_tCurveColorB );
				}
			}
			
			AddPreset( pPreset );
		}
	}

	script.CloseFile();

	return true;
}


//r3dTL::TArray< LightPreset >	LightPreset::m_dPresets;
HashTableDynamic< LightPreset *, FixedString, LightPreset::LightPresetHashFunc_T, 256	>	LightPreset::m_dPresets;


static int FrozenTexSizeToSizeIdx( int size )
{
	if( size <= 128 )
		return 0 ;

	if( size <= 256 )
		return 1 ;

	if( size <= 512 )
		return 2 ;

	if( size <= 1024 )
		return 3 ;

	return 0 ;
}

static int FrozenSizeIdxToTexSize( int idx )
{
	switch( idx )
	{
	default:
	case 0:
		return 128 ;
	case 1:
		return 256 ;
	case 2:
		return 512 ;
	case 3:
		return 1024 ;

	}
}


#ifndef FINAL_BUILD

bool IsAllLightOfType( const GameObjects& selected, int Type )
{
	for( int i = 0, e = selected.Count() ; i < e ; i ++ )
	{
		obj_LightHelper* olh = (obj_LightHelper*)selected[ i ] ;

		if( olh->LT.Type != Type )
			return false ;
	}

	return true ;
}

bool IsAllLightNotOfType( const GameObjects& selected, int Type )
{
	for( int i = 0, e = selected.Count() ; i < e ; i ++ )
	{
		obj_LightHelper* olh = (obj_LightHelper*)selected[ i ] ;

		if( olh->LT.Type == Type )
			return false ;
	}

	return true ;
}


float	obj_LightHelper :: DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( IsParentOrEqual( &ClassData, startClass ) )
	{
		starty += imgui_Value_Slider(scrx, starty, "Start Time", &startTime, 0.0f, 24.0f, "%0.2f");
		starty += imgui_Value_Slider(scrx, starty, "End Time", &endTime, 0.0f, 24.0f, "%0.2f");

		static int applyToAllSelected = 0;
		starty += imgui_Checkbox(scrx, starty, "Apply To All", &applyToAllSelected, 1);
		if(applyToAllSelected)
		{
			PropagateChange( startTime, &obj_LightHelper::startTime, selected ) ;
			PropagateChange( endTime, &obj_LightHelper::endTime, selected ) ;
			applyToAllSelected = 0;
		}

		static float rval ;

		rval = outerRadius ;
		if (!IsAllLightOfType( selected, R3D_VOLUME_LIGHT ))
		{
			starty += imgui_Value_Slider(scrx, starty, "Outer Radius", &rval, 0, 100, "%-03.2f", 1);
			PropagateChange( rval, &obj_LightHelper::outerRadius, this, selected ) ;

			static float irval ;
			irval = innerRadius ;
			starty += imgui_Value_Slider(scrx, starty, "Inner Radius", &irval, 0, outerRadius * r3dLight::MAX_INNER_OUTER_RADIUS_RATIO, "%-03.2f", 1);
			PropagateChange( irval, &obj_LightHelper::innerRadius, this, selected ) ;

			__EditorLastLightRadius = outerRadius;
		}

		static r3dColor24 col ;
		col = Color ;
		starty += imgui_DrawColorPicker(scrx, starty, "Color", &col, 360, 0);
		PropagateChange( col, &obj_LightHelper::Color, this, selected ) ;

		if( IsAllLightNotOfType( selected, R3D_TUBE_LIGHT ) && !IsAllLightOfType(selected, R3D_VOLUME_LIGHT) )
		{
			static float inVal ;
			inVal = LT.Intensity ;
			starty += imgui_Value_Slider(scrx, starty, "Intensity", &inVal, 0.f, 16.f, "%-03.2f", 1 );
			PropagateChange( inVal, &obj_LightHelper::LT, &r3dLight::Intensity, this, selected ) ;
		}

		if (IsAllLightOfType(selected, R3D_VOLUME_LIGHT))
		{
			static float inVal ;
			inVal = LT.Intensity;
			starty += imgui_Value_Slider(scrx, starty, "Attenuation", &inVal, 0.01f, 16.0f, "%-03.2f", 1 );
			PropagateChange( inVal, &obj_LightHelper::LT, &r3dLight::Intensity, this, selected ) ;
		}

		if( IsAllLightOfType( selected, R3D_OMNI_LIGHT ) )
		{
			int check = LT.bDiffuseOnly ;
			starty += imgui_Checkbox( scrx, starty, "Diffue Only", &check, 1 );
			PropagateChange( check, &obj_LightHelper::LT, &r3dLight::bDiffuseOnly, this, selected ) ;
		}

		for( int i = 0, e = selected.Count() ; i < e; i ++ )
		{
			((obj_LightHelper*)selected[ i ])->LT.Conform();
		}

		int check = LT.bCastShadows ;
		if (!IsAllLightOfType( selected, R3D_VOLUME_LIGHT ))
		{
			starty += imgui_Checkbox( scrx, starty, "Casts Shadow", &check, 1 );

			if( check )
			{
				for( int i = 0, e = (int)selected.Count(); i < e; i ++ )
				{
					obj_LightHelper* lh = static_cast<obj_LightHelper*>( selected[ i ] );

					if( !lh->LT.bCastShadows )
					{
						lh->LT.bFrozenShadowDepthDirty = 1;
					}
				}
			}

			PropagateChange( check, &obj_LightHelper::LT, &r3dLight::bCastShadows, this, selected ) ;

			if( IsAllLightOfType( selected, R3D_SPOT_LIGHT ) )
			{
				if( r_hardware_shadow_method->GetFloat() == HW_SHADOW_METHOD_R32F 
						||
					LT.bIsShadowDepthFrozen )
				{
					static float val = 0.f;
					val = r_spot_light_shadow_bias_pcf->GetFloat();
					starty += imgui_Static( scrx, starty, "PCF Shadow Offset(Global)" );
					starty += imgui_Value_Slider( scrx, starty, "Value", &val, 0, 0.5f, "%.5f" );
					r_spot_light_shadow_bias_pcf->SetFloat( val );
				}
				else
				{
					static float val = 0.f;
					val = r_spot_light_shadow_bias_hw->GetFloat();
					starty += imgui_Static( scrx, starty, "HW Shadow Offset(Global)" );
					starty += imgui_Value_Slider( scrx, starty, "Value", &val, 0, 0.125f, "%.5f" );
					r_spot_light_shadow_bias_hw->SetFloat( val );
				}

				starty += 7;
			}

			check = LT.bSSShadowBlur ;
			starty += imgui_Checkbox( scrx, starty, "Blur Shadows", &check, 1 );
			PropagateChange( check, &obj_LightHelper::LT, &r3dLight::bSSShadowBlur, this, selected );

			if( LT.bSSShadowBlur )
			{
				check = LT.bUseGlobalSSSBParams ;
				starty += imgui_Checkbox( scrx + 5, starty, "Use Global SSSB", &check, 1 );
				PropagateChange( check, &obj_LightHelper::LT, &r3dLight::bUseGlobalSSSBParams, this, selected ) ;

				if( !LT.bUseGlobalSSSBParams )
				{
					static float biasVal ;
					biasVal = LT.SSSBParams.Bias ;
					starty += imgui_Value_Slider( scrx + 5, starty, "Blur Bias",	&biasVal,		0.0f,	0.5f,		"%-02.2f", 1 );
					PropagateChange( biasVal, &obj_LightHelper::LT, &r3dLight::SSSBParams, &r3dSSSBParams::Bias , this, selected ) ;

					static float physRangeVal ;
					physRangeVal = LT.SSSBParams.PhysRange ;
					starty += imgui_Value_Slider( scrx + 5, starty, "Physicality",	&physRangeVal,	0.0f,	1024.0f,	"%-02.2f", 1 );
					PropagateChange( physRangeVal, &obj_LightHelper::LT, &r3dLight::SSSBParams, &r3dSSSBParams::PhysRange , this, selected ) ;

					static float senseVal ;
					senseVal =  LT.SSSBParams.Sense ;
					starty += imgui_Value_Slider( scrx + 5, starty, "Depth Sens.",	&senseVal,		0.0f,	1024.0f,	"%-02.2f", 1 );
					PropagateChange( senseVal, &obj_LightHelper::LT, &r3dLight::SSSBParams, &r3dSSSBParams::Sense , this, selected ) ;

					static float radiusVal ;
					radiusVal =  LT.SSSBParams.Radius ;
					starty += imgui_Value_Slider( scrx + 5, starty, "Radius",		&radiusVal,		0.0f,	12.0f,		"%-02.2f", 1 );
					PropagateChange( radiusVal, &obj_LightHelper::LT, &r3dLight::SSSBParams, &r3dSSSBParams::Radius , this, selected ) ;
				}
			}
		}

		if ( IsAllLightOfType( selected, R3D_OMNI_LIGHT ) && !LT.bDiffuseOnly )
		{
			int check = this->LT.SSScatParams.On;
			starty += imgui_Checkbox( scrx, starty, 360, 22, "SSS Enable", &check, 1 ) ;
			PropagateChange( check, &obj_LightHelper::LT, &r3dLight::SSScatParams, &r3dSSScatterParams::On, this, selected ) ;

			if( LT.SSScatParams.On )
			{				
				static float scaleVal ;
				scaleVal = LT.SSScatParams.scale ;
				starty += imgui_Value_Slider( scrx + 5, starty, "Scale", &scaleVal, 0.0f, 4.0f,	"%-02.02f",1);
				PropagateChange( scaleVal, &obj_LightHelper::LT, &r3dLight::SSScatParams, &r3dSSScatterParams::scale, this, selected ) ;

				static float distortionVal ;
				distortionVal = LT.SSScatParams.distortion ;
				starty += imgui_Value_Slider( scrx + 5, starty, "Distort", &distortionVal, 0.0f, 2.0f,	"%-02.02f",1);
				PropagateChange( distortionVal, &obj_LightHelper::LT, &r3dLight::SSScatParams, &r3dSSScatterParams::distortion, this, selected ) ;

				static float powerVal ;
				powerVal = LT.SSScatParams.power ;
				starty += imgui_Value_Slider( scrx + 5, starty, "Power", &powerVal, 0.01f, 16.0f,	"%-02.02f",1);
				PropagateChange( powerVal, &obj_LightHelper::LT, &r3dLight::SSScatParams, &r3dSSScatterParams::power, this, selected ) ;

				static float ambientVal ;
				ambientVal = LT.SSScatParams.ambient ;
				starty += imgui_Value_Slider( scrx + 5, starty, "Ambient", &ambientVal, 0.0f, 1.0f,	"%-02.02f",1);
				PropagateChange( ambientVal, &obj_LightHelper::LT, &r3dLight::SSScatParams, &r3dSSScatterParams::ambient, this, selected ) ;

				static r3dColor24 translucencyVal ;
				translucencyVal = LT.SSScatParams.translucency ;
				starty += imgui_DrawColorPicker( scrx + 5, starty, "Translucency Color", &translucencyVal, 360, 1 );
				PropagateChange( translucencyVal, &obj_LightHelper::LT, &r3dLight::SSScatParams, &r3dSSScatterParams::translucency, this, selected ) ;
			}
		}
		else
		{
			LT.SSScatParams.On = 0 ;
		}

		if ( IsAllLightOfType( selected, R3D_SPOT_LIGHT ) )
		{
			static float spotAngleOuterVal ;
			spotAngleOuterVal = LT.SpotAngleOuter ;
			starty += imgui_Value_Slider( scrx, starty, "Outer Angle", &spotAngleOuterVal, 0, 90, "%-02.2f", 1 );
			PropagateChange( spotAngleOuterVal, &obj_LightHelper::LT, &r3dLight::SpotAngleOuter, this, selected ) ;

			static float spotAngleInnerVal ;
			spotAngleInnerVal = LT.SpotAngleInner ;
			starty += imgui_Value_Slider( scrx, starty, "Inner Angle", &spotAngleInnerVal, 0, LT.SpotAngleOuter - 0.33f, "%-02.2f", 1 );
			PropagateChange( spotAngleInnerVal, &obj_LightHelper::LT, &r3dLight::SpotAngleInner, this, selected ) ;

			static float spotAngleFalloffPow ;
			spotAngleFalloffPow = LT.SpotAngleFalloffPow ;
			starty += imgui_Value_Slider( scrx, starty, "Falloff Pow", &spotAngleFalloffPow, 0.05f, 32, "%-02.2f", 1 );
			PropagateChange( spotAngleFalloffPow, &obj_LightHelper::LT, &r3dLight::SpotAngleFalloffPow, this, selected ) ;

			extern	r3dPoint3D	UI_TargetPos;		// snapped to object position (object center)
			if (Keyboard->IsPressed(kbsLeftAlt))
			{
				r3dVector V = UI_TargetPos - GetPosition();

				LT.Direction = V;
				LT.Direction.Normalize();

				D3DXMATRIX mtx;
				r3dComposeLightMatrix( mtx, &LT );

				float pitch = asin( -mtx.m[2][1] );
				float yaw = 0;

				if ( pitch < R3D_PI_2 )
				{
					if ( pitch > -R3D_PI_2 )
						yaw = atan2( mtx.m[2][0], mtx.m[2][2] );
					else
						yaw = -atan2(-mtx.m[1][0],mtx.m[0][0]);
				}
				else
				{
					yaw = atan2(-mtx.m[1][0],mtx.m[0][0]);
				}

				r3dVector angles (R3D_RAD2DEG( yaw ), R3D_RAD2DEG( pitch ), 0) ;
				PropagateChange( angles, &obj_LightHelper::SetRotationVector, selected ) ;
			}
		}

		if( IsAllLightOfType( selected, R3D_TUBE_LIGHT ) || IsAllLightOfType( selected, R3D_PLANE_LIGHT ) )
		{
			static float len, wid ;
			len = LT.GetLength() ;
			starty += imgui_Value_Slider( scrx, starty, "Length", &len, 0, 90, "%-02.2f", 1 );
			wid = LT.GetWidth() ;

			if( IsAllLightOfType( selected, R3D_PLANE_LIGHT ) )
			{
				starty += imgui_Value_Slider( scrx, starty, "Width", &wid, 0, 90, "%-02.2f", 1 );
				starty += imgui_Checkbox( scrx, starty, 360, 22, "Double Sided", &LT.bDoubleSided, 1 );
			}

			for( int i = 0, e = selected.Count() ; i < e; i ++ )
			{
				((obj_LightHelper*)selected[ i ])->LT.SetWidthLength( wid, len );
			}
		}

		if(		IsAllLightOfType( selected, R3D_OMNI_LIGHT ) 
					||
				IsAllLightOfType( selected, R3D_SPOT_LIGHT ) 
			)
		{
			if( selected.Count() <= 1 )
			{
				int prevFrozen = LT.bIsShadowDepthFrozen ;

				starty += imgui_Checkbox( scrx, starty, "Force Freeze Shadow", &LT.bIsShadowDepthFrozen, 1 ) ;

				{
					starty += imgui_Static( scrx, starty, "Frozen ShadowMap Size" ) ;

					stringlist_t names ;

					names.push_back( "128x128" ) ;
					names.push_back( "256x256" ) ;
					names.push_back( "512x512" ) ;
					names.push_back( "1024x1024" ) ;

					float TEX_DIM_LIST_SIZE = 180.f ;

					int size_idx = FrozenTexSizeToSizeIdx( LT.FrozenShadowDepthSize ) ;

					float dummyOffset = 0.f ;

					starty += imgui_DrawList( scrx, starty,  360, TEX_DIM_LIST_SIZE, names, &dummyOffset, &size_idx ) ;

					int newSize = FrozenSizeIdxToTexSize( size_idx ) ;

					if( newSize != LT.FrozenShadowDepthSize )
					{
						LT.FrozenShadowDepthSize	= newSize ;
						LT.bFrozenShadowDepthDirty	= 1 ;
					}

					starty += TEX_DIM_LIST_SIZE ;
				}

				if( prevFrozen != LT.bIsShadowDepthFrozen )
				{
					LT.UpdateDepthMap();

					if( LT.bIsShadowDepthFrozen )
					{
						LT.bFrozenShadowDepthDirty = 1 ;
					}
				}
			}
			else
			{
				starty += imgui_Static( scrx, starty, "Select 1 light to configure Freeze" ) ;
			}
		}

		if( selected.Count() > 1 )
		{
			return starty-scry ;
		}

		//------------------------------------------------------------------------
		// Projection Texture

		starty += 10 + 2;
		const int THUMB_SIZE = 100;

		imgui_DrawPanel ( scrx, starty - 4, 360, THUMB_SIZE + 4 );

		if( LT.ProjectMap )
		{
			char full[ 256 ], drive[ 8 ], file[ 128 ], ext[ 32 ];

			_splitpath( LT.ProjectMap->getFileLoc().FileName, drive, full, file, ext );

			sprintf( full, "%s%s", file, ext );

			imgui_Static(scrx + 22 + 100, starty,		full, 220 );

			r3dDrawBox2D(scrx + 10 + 2 - Desktop().GetX(), starty - Desktop().GetY(),	THUMB_SIZE-4, THUMB_SIZE-4, r3dColor(255,255,255),  LT.ProjectMap );
		}
		else
		{
			r3dDrawBox2D(scrx+10+2 - Desktop().GetX(),starty - Desktop().GetY(), THUMB_SIZE-4, THUMB_SIZE-4, r3dColor(255,255,255));
		}

		const float CLEAR_BTN_HT = 20.f;

		if (imgui_Button(scrx+22.0f+100.0f,starty + ( LT.ProjectMap ? 30.0f : 0.0f ), 220.0f, CLEAR_BTN_HT, "CLEAR", 0))
		{
			r3dRenderer->DeleteTexture( LT.ProjectMap );
			LT.ProjectMap = NULL;
		}

		starty += THUMB_SIZE;


		//starty += CLEAR_BTN_HT;

		g_pDesktopManager->End();

		static char sProjTexName[128] ;

		if( LT.ProjectMap )
		{
			char drive[16], dir[ 512 ], name[512], ext[ 512 ] ;
			_splitpath( LT.ProjectMap->getFileLoc().FileName, drive, dir, name, ext ) ;
			strcpy( sProjTexName, name ) ;
			strcat( sProjTexName, ext ) ;
		}

#define PROJ_TEXTURES_FOLDER "Data\\ProjectionTextures\\"

		static float offset = 0.f ;

		imgui_Static( 5 + 150 + 10, r3dRenderer->ScreenH - 420, "Proj. Texture", 150, true, 20 ) ;

		if (imgui_FileList(5 + 150 + 10, r3dRenderer->ScreenH - 400, 150, 200, PROJ_TEXTURES_FOLDER"*.*", sProjTexName, &offset, false))
		{
			char sFull[128];
			sprintf( sFull, "%s%s", PROJ_TEXTURES_FOLDER, sProjTexName );

			if( LT.ProjectMap )
				r3dRenderer->DeleteTexture( LT.ProjectMap );

			LT.ProjectMap = r3dRenderer->LoadTexture( sFull );
		}


		g_pDesktopManager->Begin( "ed_objects" );

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//	>	
		//
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		static LightPreset preset_edit;

		//starty += imgui_Static (scrx, starty, "Light presets:" );

		static char buffer[ MAX_PATH ] = "";

		imgui2_StartArea ( "BlaBla", scrx, starty, 220, 24 );
		imgui2_StringValue( "Light presets:", buffer );
		imgui2_EndArea ();

		if ( imgui_Button ( scrx + 220, starty, 70, 24, "Add" ) )
		{
			if ( *buffer &&  strcmp( UNUSED_PRESET_NAME, buffer ) )
			{
				LightPreset * pPreset = gfx_new LightPreset( preset_edit );
				pPreset->SetName( buffer );
				LightPreset::AddPreset( pPreset );
			}
		}

		if ( imgui_Button ( scrx + 290, starty, 70, 24, "Del" ) )
		{
			r3dOutToLog( "Not implemented!" );
		}
		starty+= 25;


		static stringlist_t str_lst;

		str_lst.clear();
		str_lst.push_back( UNUSED_PRESET_NAME );
		LightPreset::FillNamesList( str_lst );

		float			fOffset = 0.0f;
		static int nIdx = 0;
		g_pDesktopManager->End();


		if ( m_pPreset )
		{
			for ( uint32_t i = 1; i < str_lst.size(); i++ )
			{
				if ( str_lst[ i ] != m_pPreset->GetName() )
					continue;

				nIdx = i;

				preset_edit.CopyFrom( *m_pPreset );
				r3dscpy( buffer, m_pPreset->GetName() );
				break;
			}		
		}


		bool bSelectChange = false;
		float fPosY = r3dRenderer->ScreenH - 420;
		if ( imgui_DrawList( 5, fPosY, 150, 180, str_lst, &fOffset, &nIdx, false))
		{
			bSelectChange = true;

			if ( nIdx >= 0 )
				r3dscpy( buffer, str_lst[ nIdx ].c_str() );

			m_pPreset = LightPreset::FindPreset( buffer );
		}
		fPosY += 180;

		if ( imgui_Button ( 5, fPosY, 75, 24, "Save" ) )
		{
			LightPreset::SaveToScript( FNAME_LIGHT_PRESETS );
		}

		if ( imgui_Button ( 80, fPosY, 75, 24, "Load" ) )
		{
			LightPreset::LoadFromScript( FNAME_LIGHT_PRESETS );
			bSelectChange = true;
			if ( str_lst.size() )
			{
				nIdx = nIdx >= (int)str_lst.size() ? 0 : nIdx;
				r3dscpy( buffer, str_lst[ nIdx ].c_str() );
				preset_edit.Reset();
			}
		}


		g_pDesktopManager->Begin( "ed_objects" );


		starty += imgui_Value_Slider(scrx, starty, "Time", preset_edit.GetTimePtr(),	0, 100,	"%-02.2f",1) + 1;

		starty += imgui_Value_Slider(scrx, starty, "Light radius scale", preset_edit.GetRadiusSclPtr(),	0, 10,	"%-02.2f",1) + 1;
		imgui_DrawFloatGradient( scrx, starty, "Light radius", preset_edit.GetCurveRadiusPtr(), 360, 200, 0, 2.f, 10, 10, 1, 1 );
		starty += 210;
		imgui_DrawFloatGradient( scrx, starty, "Color Red", preset_edit.GetCurveColorRPtr(), 360, 200, 0, 2.f, 10, 10, 1, 1 );
		starty += 210;
		imgui_DrawFloatGradient( scrx, starty, "Color Green", preset_edit.GetCurveColorGPtr(), 360, 200, 0, 2.f, 10, 10, 1, 1 );
		starty += 210;
		imgui_DrawFloatGradient( scrx, starty, "Color Blue", preset_edit.GetCurveColorBPtr(), 360, 200, 0, 2.f, 10, 10, 1, 1 );
		starty += 210;


		if ( nIdx > 0 )
		{
			LightPreset * pPreset = LightPreset::FindPreset( buffer );
			if ( pPreset )
			{
				if ( bSelectChange )
				{
					preset_edit.CopyFrom( *pPreset );
				}
				else
				{
					pPreset->CopyFrom( preset_edit );
				}
			}
		}
		else
			preset_edit.Reset();
	}


	return starty-scry;
}
#endif

BOOL obj_LightHelper::OnDestroy()
{
	if (LT.id != 0xffffffff)
		WorldLightSystem.Remove(&LT); 

	return parent::OnDestroy();
}


BOOL obj_LightHelper::Update()
{
	if (bKilled) 
		return FALSE;

	float fRes = 0.f;
	r3dPoint3D vColor( Color.R, Color.G, Color.B );
	if ( m_pPreset )
	{
		float f  = LT.fTime / m_pPreset->GetTime();
		float fFactor = f - floorf( f );

		fRes = m_pPreset->GetCurveRadius().GetValue( fFactor ).x * m_pPreset->GetRadiusScl();
		vColor.x *= m_pPreset->GetCurveColorR().GetValue( fFactor ).x;
		vColor.y *= m_pPreset->GetCurveColorG().GetValue( fFactor ).x;
		vColor.z *= m_pPreset->GetCurveColorB().GetValue( fFactor ).x;
	}

#ifndef FINAL_BUILD
	if(!g_bEditMode || g_bForceQualitySelectionInEditor)
#endif
	{
		// r_lighting_quality can only be 1 or 2, so cast lights quality to those numbers firstly
		int lightQuality = (int)m_MinQualityLevel;
		if(lightQuality > (int)r_lighting_quality->GetMaxVal()) lightQuality = (int)r_lighting_quality->GetMaxVal();
		LT.m_bQualityDisabled = (lightQuality > r_lighting_quality->GetInt());
	}

 LT.Assign(GetPosition().X,GetPosition().Y,GetPosition().Z);
 LT.SetRadius(innerRadius, outerRadius + fRes);
 LT.SetColor( vColor.x, vColor.y, vColor.z );

 float curGTime = r3dGameLevel::Environment->__CurTime;
 if(startTime < endTime) // day lights
 {
	 if(curGTime < startTime || curGTime > endTime)
	 {
		 bOn = false;
	 }
	 else
	 {
		 bOn = true;
	 }
 }
 else // night lights
 {
	 if(curGTime >= endTime && curGTime <= (startTime))
	 {
		 bOn = false;
	 }
	 else 
	 {
		 bOn = true;
	 }
 }

 if(bOn && !LT.IsOn())
	 LT.TurnOn();
 else if(!bOn && LT.IsOn())
	 LT.TurnOff();

#ifndef FINAL_BUILD
 if( g_bEditMode )
 {
   // prevent scaling cause it's not obvious with light object yet can distort scene box significantly
   SetScale( r3dPoint3D( 1.f, 1.f, 1.f ) );

   float sx = 1.0f;
   float sy = 1.0f;
   float sz = 1.0f;

   D3DXVECTOR4 viewZ ( r3dRenderer->ViewMatrix._13, r3dRenderer->ViewMatrix._23, r3dRenderer->ViewMatrix._33, r3dRenderer->ViewMatrix._43 );
   D3DXVECTOR4 hpos( GetPosition().x, GetPosition().y, GetPosition().z, 1.0f );

   float Z = D3DXVec4Dot( &hpos, &viewZ );

   Z = R3D_MAX( Z, 1.f );

   sx = R3D_MAX( sx, .0433f * Z );
   sy = R3D_MAX( sy, .0433f * Z );
   sz = R3D_MAX( sz, .0433f * Z );

   r3dBoundBox bboxLocal ;

   bboxLocal.Size = r3dPoint3D( sx, sy, sz );
   bboxLocal.Org = -bboxLocal.Size * 0.5f;

   SetBBoxLocal( bboxLocal );

   GameObject::UpdateTransform();
 }
#endif

 return TRUE;
}

GameObject * obj_LightHelper::Clone ()
{
	obj_LightHelper * pNew = (obj_LightHelper*)srv_CreateGameObject("obj_LightHelper", FileName.c_str(), GetPosition () );

	pNew->SetRotationVector(GetRotationVector());
	pNew->m_pPreset = m_pPreset;
	pNew->Color = Color;
	pNew->LT = LT;
	pNew->bOn = bOn;
	pNew->startTime = startTime;
	pNew->endTime= endTime;
	pNew->innerRadius = innerRadius;
	pNew->outerRadius = outerRadius;
	pNew->bKilled = bKilled;

	pNew->bSerialized = true;

	return pNew;
}


BOOL obj_LightHelper::OnEvent(int event, void *data)
{
 
 return FALSE;
}







void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size)
{
	r3dPoint3D scr;
	if(!r3dProjectToScreen(pos, &scr))
		return;

	float TargetBoxX = scr.x;
	float TargetBoxY = scr.y;
	
	r3dDrawBox2D(TargetBoxX - size/2,TargetBoxY - size/2,size,size,Col, icon);
}

struct LightHelperRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		LightHelperRenderable *This = static_cast<LightHelperRenderable*>( RThis );

		This->Parent->DoDraw();
	}

	obj_LightHelper* Parent;	
};

/*virtual*/
void
obj_LightHelper::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) /*OVERRIDE*/
{
#ifdef FINAL_BUILD
	return;
#else
	if(!r_show_light_helper->GetInt() )
		return;

	if( r_hide_icons->GetInt() )
		return ;

	float idd = r_icons_draw_distance->GetFloat();
	idd *= idd;

	if( ( Cam - GetPosition() ).LengthSq() > idd )
		return;

	extern int CurHUDID;
	if(CurHUDID !=0)
		return;

	if(LT.m_bQualityDisabled)
		return;

	LightHelperRenderable rend;

	rend.Init();

	rend.Parent		= this;
	rend.SortValue	= 17 * RENDERABLE_USER_SORT_VALUE;

	render_arrays[ rsDrawComposite1 ].PushBack( rend );
#endif
}

void
obj_LightHelper::DoDraw()
{
	r3dColor Cl = r3dRenderer->AmbientColor;
	if (bOn) 
		Cl = Color;

	r3dRenderer->SetTex(NULL);
	r3dRenderer->SetMaterial(NULL);

	r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC );

	if((LT-gCam).Length() < 200)
	{
		//r3dDrawIcon3D(LT, NULL, Cl, 24);
		r3dDrawIcon3D(LT, LampIcon, r3dColor(255,255,255), 32);
	}

	r3dRenderer->SetTex(NULL);
	r3dRenderer->SetMaterial(NULL);

#ifndef FINAL_BUILD
	if ( g_Manipulator3d.IsSelected( this ) )
	{
		D3DXMATRIX mtx;
		D3DXMATRIX scale;
		D3DXMATRIX final;

		//r3dDrawIcon3D(LT, NULL, Cl, 32);
		r3dDrawIcon3D(LT, LampIcon, r3dColor(255,255,255), 32);

		switch( LT.Type )
		{
		case R3D_SPOT_LIGHT:
			{
				r3dGetSpotLightScaleMatrix( scale, &LT );
				r3dComposeLightMatrix( mtx, &LT );
				D3DXMatrixMultiply( &final, &scale, &mtx );

				r3dMeshSetShaderConsts( final, NULL );

				r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ );
				r3dDrawConeLines( r3dColor( 0x7f, 0x7f, 0x7f, 0x88 ) );
				r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
				r3dDrawConeLines( r3dColor( 0xff, 0x33, 0x33, 0xff ) );

				float innerRad, innerZScale;
				r3dGetSpotLightInnerScaleAttribs( innerRad, innerZScale, &LT );
				r3dGetSpotLightScaleMatrix( scale, innerRad, innerZScale );
				D3DXMatrixMultiply( &final, &scale, &mtx );
				r3dMeshSetShaderConsts( final, NULL );

				r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ );
				r3dDrawConeLines( r3dColor( 0x7f, 0x7f, 0x7f, 0x88 ) );
				r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
				r3dDrawConeLines( r3dColor( 0xff, 0xff, 0x33, 0xff ) );

			}
			break;

		case R3D_OMNI_LIGHT:
			{
				D3DXMatrixTranslation( &mtx, LT.x, LT.y, LT.z );
				D3DXMatrixScaling( &scale, LT.Radius2, LT.Radius2, LT.Radius2 );

				D3DXMatrixMultiply( &final, &scale, &mtx );

				r3dMeshSetShaderConsts( final, NULL );

				r3dDrawGeoSpheresStart();

				r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

				r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ );						
				r3dSetFwdColorShaders( r3dColor( 0x7f, 0x7f, 0x7f, 0x55 ) );
				r3dDrawGeoSphere();
				
				r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
				r3dSetFwdColorShaders( r3dColor( 0xff, 0x33, 0x33, 0xff ) );
				r3dDrawGeoSphere();	

				r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

				r3dDrawGeoSpheresEnd();						
			}
			break;

		case R3D_TUBE_LIGHT:
			{
				D3DXMATRIX wm ;
				LT.GetTubeWorldMatrix( &wm, true ) ;

				r3dMeshSetShaderConsts( wm, NULL );

				r3dDrawGeoSpheresStart();

				r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

				r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ );						
				r3dSetFwdColorShaders( r3dColor( 0x7f, 0x7f, 0x7f, 0x55 ) );
				r3dDrawGeoSphere();

				r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
				r3dSetFwdColorShaders( r3dColor( 0xff, 0x33, 0x33, 0xff ) );
				r3dDrawGeoSphere();	

				r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

				r3dDrawGeoSpheresEnd();

				D3DXVECTOR4 tpos0 ( - 0.5f * LT.GetLength(), 0.f, 0.f, 1.f );
				D3DXVECTOR4 tpos1 ( + 0.5f * LT.GetLength(), 0.f, 0.f, 1.f );

				D3DXMATRIX tmat ;

				D3DXMatrixIdentity( &tmat );

				r3dMeshSetShaderConsts( tmat, NULL );

				LT.GetTubeWorldMatrix( &tmat, false );

				D3DXVec4Transform( &tpos0, &tpos0, &tmat );
				D3DXVec4Transform( &tpos1, &tpos1, &tmat );

				r3dPoint3D p0 = (r3dPoint3D&)tpos0 ;
				r3dPoint3D p1 = (r3dPoint3D&)tpos1 ;

				r3dSetFwdColorShaders( r3dColor( LT.R, LT.G, LT.B ) );

				r3dDrawLine3D( p0, p1, gCam, ( 0.5f * LT.GetLength() + LT.Radius2 ) / 64, r3dColor::black );

				r3dSetFwdColorShaders( r3dColor::green );

#if 0
				r3dDrawBoundBox( LT.BBox, gCam, r3dColor::black, 0.125f );
#endif

				r3dRenderer->Flush();
			}
			break ;

		case R3D_PLANE_LIGHT:
			{
				D3DXMATRIX wm ;

				LT.GetPlaneWorldMatrix( &wm, true );

				r3dMeshSetShaderConsts( wm, NULL );

				r3dDrawChamferBoxStart();

				r3dRenderer->SetCullMode( D3DCULL_NONE );
				r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

				r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ );
				r3dSetFwdColorShaders( r3dColor( 0x7f, 0x7f, 0x7f, 0x55 ) );
				r3dDrawChamferBox();

				r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );
				r3dSetFwdColorShaders( r3dColor( 0xff, 0x33, 0x33, 0xff ) );
				r3dDrawChamferBox();

				r3dRenderer->RestoreCullMode();
				r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

				r3dDrawChamferBoxEnd();
			}
			break ;
		}

		r3dRenderer->SetTex(NULL);
		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode(R3D_BLEND_COPY_ZCW);

	}
#endif
}

void obj_LightHelper::SelectLightType()
{
	LT.SetType(R3D_OMNI_LIGHT);

	if (strstr(Name.c_str(), "Spot") )
	{
		LT.SetType(R3D_SPOT_LIGHT);
	}
	else
	if( strstr( Name.c_str(), "Tube") )
	{
		LT.SetType( R3D_TUBE_LIGHT );
	}
	else
	if( strstr( Name.c_str(), "Plane" ) )
	{
		LT.SetType( R3D_PLANE_LIGHT );
	}
	else
	if( strstr( Name.c_str(), "Volume" ) )
	{
		LT.SetType( R3D_VOLUME_LIGHT );
	}
}

template< bool W >
void Serialize( pugi::xml_node node, r3dSSScatterParams* params ) ;

void obj_LightHelper::ReadSerializedData(pugi::xml_node& node)
{	
	SelectLightType();

	GameObject::ReadSerializedData(node);

	pugi::xml_node lightNode = node.child("lightHelper");
	//	New version of light distance parameters specification
	if (lightNode.attribute("radius").empty())
	{
		outerRadius = lightNode.attribute("outerRadius").as_float();
		innerRadius = lightNode.attribute("innerRadius").as_float();

		innerRadius = R3D_MIN( innerRadius, outerRadius * r3dLight::MAX_INNER_OUTER_RADIUS_RATIO );
	}
	//	Old version of light distance parameters specification
	else
	{
		outerRadius = lightNode.attribute("radius").as_float();
		innerRadius = 0.0f;
	}

	if(!lightNode.attribute("startTime").empty())
	{
		startTime = lightNode.attribute("startTime").as_float();
		endTime = lightNode.attribute("endTime").as_float();
	}

	if( g_leveldata_xml_ver->GetInt() < 100 )
	{
		// fix radius bug in old versions
		outerRadius += 1.f ;
	}

	Color.R = lightNode.attribute("color_r").as_int();
	Color.G = lightNode.attribute("color_g").as_int();
	Color.B = lightNode.attribute("color_b").as_int();


	char buffer[ MAX_PATH ];
	assert( ! m_pPreset );
	r3dscpy( buffer, lightNode.attribute( "light_preset" ).value() );
	if ( *buffer )
		m_pPreset = LightPreset::FindPreset( buffer );


	// paranoia sets in
	LT.bDiffuseOnly = 0 ;

	LT.IsLevelLight = 1;

	pugi::xml_attribute attrib;

	if( LT.Type == R3D_OMNI_LIGHT && lightNode.attribute( "intensity" ).empty() )
	{
		LT.Intensity = 4.f ;
	}

#define PUGI_GET_IF_SET(var,name,type) attrib = lightNode.attribute(name); if( !attrib.empty() ) { var = attrib.as_##type(); }

	PUGI_GET_IF_SET( LT.bDiffuseOnly,			"diffuse_only"				, int	);
	PUGI_GET_IF_SET( LT.bCastShadows,			"casts_shadows"				, int	);
	PUGI_GET_IF_SET( LT.Intensity,				"intensity"					, float	);
	PUGI_GET_IF_SET( LT.SpotAngleOuter,			"spot_outer_angle"			, float	);
	PUGI_GET_IF_SET( LT.SpotAngleInner,			"spot_inner_angle"			, float	);
	PUGI_GET_IF_SET( LT.SpotAngleFalloffPow,	"spot_angle_falloff_pow"	, float	);
	PUGI_GET_IF_SET( LT.bIsShadowDepthFrozen,		"frozen_shadows"			, int	);
	PUGI_GET_IF_SET( LT.FrozenShadowDepthSize,	"frozen_shadows_size"		, int	);

	PUGI_GET_IF_SET( LT.bSSShadowBlur,			"sssb"						, int	);
	PUGI_GET_IF_SET( LT.bUseGlobalSSSBParams,	"global_sssb"				, int	);
	PUGI_GET_IF_SET( LT.SSSBParams.Bias,		"sssb_bias"					, float	);
	PUGI_GET_IF_SET( LT.SSSBParams.PhysRange,	"sssb_physrange"			, float	);
	PUGI_GET_IF_SET( LT.SSSBParams.Radius,		"sssb_radius"				, float	);
	PUGI_GET_IF_SET( LT.SSSBParams.Sense,		"sssb_sense"				, float	);

	attrib = lightNode.attribute("volume");
	if (!attrib.empty())
	{
		int flag = 0;
// 		if (attrib.as_int() != 0)
// 			flag = R3D_LIGHT_VOLUME;
		LT.Flags |= flag;
	}

	if( LT.Type == R3D_TUBE_LIGHT || LT.Type == R3D_PLANE_LIGHT )
	{
		float Width = 0.f ;
		float Length ;
		PUGI_GET_IF_SET( Length,	"length",	float ) ;

		if( LT.Type == R3D_PLANE_LIGHT )
		{
			PUGI_GET_IF_SET( Width, "width", float );
			PUGI_GET_IF_SET( LT.bDoubleSided, "double_sided", int );

			LT.bDoubleSided = !!LT.bDoubleSided ;
		}

		LT.SetWidthLength( Width, Length );
	}


#undef PUGI_GET_IF_SET

	attrib = lightNode.attribute( "projection_tex" ); 
	
	if( !attrib.empty() ) 
	{
		LT.ProjectMap = r3dRenderer->LoadTexture( attrib.value() );
	}

	Serialize<false>( node, &LT.SSScatParams );

	LT.Assign(GetPosition().X,GetPosition().Y,GetPosition().Z);
	LT.SetRadius(innerRadius, outerRadius);
	LT.SetColor(Color.R, Color.G, Color.B);

	LT.Conform();

	RecalcDirection() ;

	bSerialized = true;
}

void obj_LightHelper::WriteSerializedData(pugi::xml_node& node)
{
#ifndef FINAL_BUILD
	LT.Conform();

	GameObject::WriteSerializedData(node);
	pugi::xml_node lightNode = node.append_child();
	lightNode.set_name("lightHelper");
	lightNode.append_attribute("outerRadius")			= outerRadius;
	lightNode.append_attribute("innerRadius")			= R3D_MIN( innerRadius, outerRadius * r3dLight::MAX_INNER_OUTER_RADIUS_RATIO );
	lightNode.append_attribute("startTime") = startTime;
	lightNode.append_attribute("endTime") = endTime;
	lightNode.append_attribute("color_r")				= Color.R;
	lightNode.append_attribute("color_g")				= Color.G;
	lightNode.append_attribute("color_b")				= Color.B;
	lightNode.append_attribute("intensity")				= LT.Intensity;
	lightNode.append_attribute("diffuse_only")			= LT.bDiffuseOnly;
	lightNode.append_attribute("casts_shadows")			= LT.bCastShadows;
	lightNode.append_attribute("frozen_shadows")		= LT.bIsShadowDepthFrozen;
	lightNode.append_attribute("frozen_shadows_size")	= LT.FrozenShadowDepthSize;

	lightNode.append_attribute( "sssb"				) = LT.bSSShadowBlur;
	lightNode.append_attribute(	"global_sssb"		) = LT.bUseGlobalSSSBParams;
	lightNode.append_attribute(	"sssb_bias"			) = LT.SSSBParams.Bias;
	lightNode.append_attribute(	"sssb_physrange"	) = LT.SSSBParams.PhysRange;
	lightNode.append_attribute(	"sssb_radius"		) = LT.SSSBParams.Radius;
	lightNode.append_attribute(	"sssb_sense"		) = LT.SSSBParams.Sense;

	if( LT.ProjectMap )
	{
		lightNode.append_attribute( "projection_tex" ) = LT.ProjectMap->getFileLoc().FileName;
	}

	if ( m_pPreset )
		lightNode.append_attribute("light_preset") = m_pPreset->GetName();

	if( LT.Type == R3D_SPOT_LIGHT )
	{
		lightNode.append_attribute("spot_outer_angle")			= LT.SpotAngleOuter;
		lightNode.append_attribute("spot_inner_angle")			= LT.SpotAngleInner;
		lightNode.append_attribute("spot_angle_falloff_pow")	= LT.SpotAngleFalloffPow;
	}

	if( LT.Type == R3D_TUBE_LIGHT || LT.Type == R3D_PLANE_LIGHT )
	{
		lightNode.append_attribute( "length" ) = LT.GetLength() ;
	}

	if( LT.Type == R3D_PLANE_LIGHT )
	{
		lightNode.append_attribute( "width" ) = LT.GetWidth() ;
		lightNode.append_attribute( "double_sided" ) = LT.bDoubleSided ? 1 : 0 ;
	}

	Serialize<true>( lightNode, &LT.SSScatParams );
#endif
}

/*virtual*/
BOOL obj_LightHelper::OnPositionChanged() /*OVERRIDE*/
{
	BOOL res = GameObject::OnPositionChanged();

	if( LT.bIsShadowDepthFrozen )
	{
		LT.bFrozenShadowDepthDirty = 1 ;
	}

	return res ;
}

BOOL obj_LightHelper::OnOrientationChanged() /*OVERRIDE*/
{
	BOOL res = GameObject::OnOrientationChanged();

	if( LT.bIsShadowDepthFrozen )
	{
		LT.bFrozenShadowDepthDirty = 1 ;
	}

	RecalcDirection() ;

	return res ;
}

void obj_LightHelper::RecalcDirection()
{
	D3DXMATRIX lightRotMat = GetRotationMatrix();
	D3DXVECTOR3 lightZ( 0, 0, 1 );
	D3DXVECTOR3 lightX( 1, 0, 0 );

	D3DXVec3TransformNormal( &lightZ, &lightZ, &lightRotMat );
	D3DXVec3TransformNormal( &lightX, &lightX, &lightRotMat );

	LT.Direction = r3dPoint3D( lightZ.x, lightZ.y, lightZ.z );
	LT.ZDirection = r3dPoint3D( lightX.x, lightX.y, lightX.z );
}
