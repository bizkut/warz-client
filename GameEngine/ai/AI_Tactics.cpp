//=========================================================================
//	Module: AI_Tactics.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#ifndef FINAL_BUILD

#include "AI_Tactics.h"
#include "r3dSpline.h"

#include "../../GameEngine/ai/AutodeskNav/AutodeskNavMesh.h"

AI_TacticConfig::AI_TacticConfig( uint32_t id )
	: m_TacticID( id )
{
}

AI_TacticConfig::~AI_TacticConfig()
{
	Destroy();
}

void AI_TacticConfig::Destroy()
{
	m_Name[0] = _T('\0');
	m_TacticID = UINT_MAX;
}

bool AI_TacticConfig::NextCheckpoint( AI_TacticParams& params ) const
{
	if( params.m_NumCheckpoints > params.m_CurrentCheckpointNdx )
		++params.m_CurrentCheckpointNdx;
	else if( UINT_MAX == params.m_CurrentCheckpointNdx )
		params.m_CurrentCheckpointNdx = 0;

	if( params.m_NumCheckpoints == params.m_CurrentCheckpointNdx )
	{
		params.m_CurrentCheckpointNdx = UINT_MAX;
		return false;
	}

	return true;
}


//---------------------------------------------------------

AI_StraightLineTacticConfig::AI_StraightLineTacticConfig( uint32_t id )
	: AI_TacticConfig( id )
{
}

void AI_StraightLineTacticConfig::Destroy()
{
	AI_TacticConfig::Destroy();
}

bool AI_StraightLineTacticConfig::GetEvalTacticParams( AI_TacticParams& params ) const
{
	// Assumption:	The params.m_DestPos and params.m_StartPos
	//				have been filled in.

	params.m_Axis = params.m_DestPos - params.m_StartPos;
	params.m_Scale = params.m_Axis.Length();
	params.m_TacticConfig = const_cast<AI_StraightLineTacticConfig*>(this);

	return true;
}

bool AI_StraightLineTacticConfig::GetExecTacticParams( AI_TacticParams& params ) const
{
	// NOTE: Call GetEvalTacticParams before
	//       calling GetExecTacticParams.

	params.m_Angle = 0.0f;
	params.m_NumCheckpoints = 1;
	return true;
}

r3dPoint3D AI_StraightLineTacticConfig::GetPosition( const AI_TacticParams& params, const float& timeSpent ) const
{
	return params.m_DestPos;
}

r3dPoint3D AI_StraightLineTacticConfig::GetPrecomputedPosition( AI_TacticParams& params, const float& timeSpent ) const
{
	return params.m_DestPos;
}

//---------------------------------------------------------

AI_BezierTacticConfig::AI_BezierTacticConfig( uint32_t id )
	: AI_TacticConfig( id )
	, m_NumPoints( 0 )
	, m_Points( NULL )
{
}

void AI_BezierTacticConfig::Destroy()
{
	if( m_Points != NULL )
		delete[] m_Points;
	m_Points = NULL;
	m_NumPoints = 0;

	AI_TacticConfig::Destroy();
}

bool AI_BezierTacticConfig::GetEvalTacticParams( AI_TacticParams& params ) const
{
	// Assumption:	The params.m_DestPos and params.m_StartPos
	//				have been filled in.

	// Assumption:	Bezier curves are defined using 1.0
	//				second as the amount of time to
	//				complete, at a speed of 1.0.

	// Scale is given by comparing the distance between
	// the new startpos and destPos to the distance
	// between the first and last point in the original
	// Bezier curve description.
	r3dVector vFacing = params.m_DestPos - params.m_StartPos;
	r3dVector vFacingOrig = m_Points[ m_NumPoints - 1 ] - m_Points[ 0 ];
	float fDistNew = vFacing.Length();
	float fDistOrig = vFacingOrig.Length();
	params.m_Scale =  fDistNew / fDistOrig;
	params.m_TacticConfig = const_cast<AI_BezierTacticConfig*>(this);

	return true;
}

bool AI_BezierTacticConfig::GetExecTacticParams( AI_TacticParams& params ) const
{
	// NOTE: Call GetEvalTacticParams before
	//       calling GetExecTacticParams.

	// Get the axis and angle of rotation, so we can make
	// the start to finish vectors co-linear, before
	// translating the final points to their destination.
	r3dVector vFacing = params.m_DestPos - params.m_StartPos;
	r3dVector vFacingOrig = m_Points[ m_NumPoints - 1 ] - m_Points[ 0 ];
	vFacing.Normalize();
	vFacingOrig.Normalize();
	float fDot = vFacingOrig.Dot( vFacing );
	//r3dOutToLog( "  Facing(%0.4f)<%0.4f, %0.4f, %0.4f>\n", vFacing.Length(), vFacing.x, vFacing.y, vFacing.z );
	//r3dOutToLog( "  FacingOrig(%0.4f)<%0.4f, %0.4f, %0.4f>\n", vFacingOrig.Length(), vFacingOrig.x, vFacingOrig.y, vFacingOrig.z );
	//r3dOutToLog( "  Dot: %0.4f\n", fDot );
	params.m_Axis = vFacingOrig.Cross( vFacing );
	if( 0.00001 > fDot && -0.00001 < fDot )
	{
		params.m_Angle = 0.0f;
	}
	else
	{
		params.m_Angle = r3dAcos( fDot );
		if ( params.m_Axis.y > 0.0f )
			params.m_Angle *= -1.0f;
	}
	if( params.m_Axis.y < 0.0f )
	{
		params.m_Axis *= -1.0f;
	}

	//r3dOutToLog( "  Axis<%0.4f, %0.4f, %0.4f>\n", params.m_Axis.x, params.m_Axis.y, params.m_Axis.z );
	//r3dOutToLog( "  Angle: %0.4f\n", params.m_Angle );

	// Pre-compute some destinations for a segmented path.
	params.m_NumCheckpoints = m_NumPoints;
	r3d_assert(0 < params.m_NumCheckpoints);
	for( uint32_t i = 0; i < params.m_NumCheckpoints; ++i )
	{
		float timeSpent = (float)(i+1) * params.m_Scale / (float)params.m_NumCheckpoints;
		params.m_Checkpoints[i] = GetPosition(params, timeSpent);
	}

	return true;
}

r3dPoint3D AI_BezierTacticConfig::GetPosition( const AI_TacticParams& params, const float& timeSpent ) const
{
	// Because the amount of time to complete the original
	// Bezier curve is assumed to be 1.0, the time to complete
	// the scaled curve will be equal to the scale.  When the
	// timeSpent is divided by the scale, it results in the
	// percentage of the curve completed.
	float pct = timeSpent / params.m_Scale;
	if( 0.0f > pct )
		pct = 0.0f;
	if( 1.0f < pct )
		pct = 1.0f;
	//r3dOutToLog( "      Pct: %0.4f  Spent: %0.4f  Scale: %0.4f\n", pct, timeSpent, params.m_Scale );
	
	// NOTE: The General Bezier Curve doesn't seem to calculate correctly atm.
	//r3dPoint3D point = r3dSpline3D::Bezier( m_Points, m_NumPoints, pct );
	r3dPoint3D point;
	if( 4 == m_NumPoints )
		point = r3dSpline3D::Bezier4( m_Points[ 0 ], m_Points[ 1 ], m_Points[ 2 ], m_Points[ 3 ], pct );
	else if ( 3 == m_NumPoints )
		point = r3dSpline3D::Bezier3( m_Points[ 0 ], m_Points[ 1 ], m_Points[ 2 ], pct );
	else if ( 2 == m_NumPoints )
		point = ( m_Points[ 1 ] - m_Points[ 0 ] ) * pct;
	else
		r3d_assert( false && "unsupported curve!" );
	//r3dOutToLog( "      O<%0.2f, %0.2f, %0.2f>\n", point.x, point.y, point.z );

	// Scale, Rotate and Translate the point, so it coincides
	// with the AI object's locality.
	point *= params.m_Scale;
	//r3dOutToLog( "      S<%0.2f, %0.2f, %0.2f>\n", point.x, point.y, point.z );
	point.RotateAroundVector( params.m_Axis, R3D_RAD2DEG(params.m_Angle) );
	//r3dOutToLog( "      R<%0.2f, %0.2f, %0.2f>\n", point.x, point.y, point.z );
	point += params.m_StartPos;
	//r3dOutToLog( "      T<%0.2f, %0.2f, %0.2f>\n", point.x, point.y, point.z );

	return point;
}

r3dPoint3D AI_BezierTacticConfig::GetPrecomputedPosition( AI_TacticParams& params, const float& timeSpent ) const
{
	//float timePerCheckPoint = params.m_Scale / (float)m_NumCheckpoints;
	//params.m_CurrentCheckpointNdx = (uint32_t)(timeSpent / timePerCheckPoint);
	
	//r3dOutToLog("!!! timeSpent: %0.2f  numCheckpoints: %d  scale: %0.2f\n", timeSpent, m_NumCheckpoints, params.m_Scale );
	//params.m_CurrentCheckpointNdx = (uint32_t)(timeSpent * (float)m_NumCheckpoints / params.m_Scale);
	r3d_assert( params.m_NumCheckpoints > params.m_CurrentCheckpointNdx );
	return params.m_Checkpoints[params.m_CurrentCheckpointNdx];
}


//---------------------------------------------------------

AI_Tactics* g_pAITactics = NULL;

AI_Tactics::AI_Tactics()
{
}

AI_Tactics::~AI_Tactics()
{
	Destroy();
}

void AI_Tactics::Init()
{
	r3d_assert( m_tacticsHash.Size() == 0 );

	// load A.I. tactics
	{
		const char* AiDBFile = "Data/AIDB.xml";
		
		r3dFile* f = r3d_open(AiDBFile, "rb");
		if ( !f )
		{
			r3dError("Failed to open %s\n", AiDBFile);
			return;
		}

		char* fileBuffer = game_new char[f->size + 1];
		r3d_assert(fileBuffer);
		fread(fileBuffer, f->size, 1, f);
		fileBuffer[f->size] = 0;
		pugi::xml_document xmlFile;
		pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
		fclose(f);
		if(!parseResult)
			r3dError("Failed to parse XML, error: %s", parseResult.description());
		pugi::xml_node xmlDB = xmlFile.child("DB");
		{
			pugi::xml_node xmlTacticsDB = xmlDB.child("TacticsDB");
			pugi::xml_node xmlTactic = xmlTacticsDB.child("Tactic");
			while(!xmlTactic.empty())
			{
				loadTactic(xmlTactic);
				xmlTactic = xmlTactic.next_sibling();
			}
		}
		delete [] fileBuffer;
	}
}

void AI_Tactics::Destroy()
{
	m_tacticsHash.IterateStart();
	while( m_tacticsHash.IterateNext() )
	{
		AI_TacticConfig* tactic = m_tacticsHash.IterateGet();
		tactic->Destroy();
	}
	m_tacticsHash.Clear();
	m_tacticsHashKeys.clear();
}

void AI_Tactics::loadTactic( pugi::xml_node& xmlTactic )
{
	r3d_assert( !xmlTactic.empty() );

	uint32_t tacticID = xmlTactic.attribute("tacticID").as_uint();
	// check if we have that tactic in our database
	{
		AI_TacticConfig* temp = NULL;
		if(m_tacticsHash.GetObject(tacticID, &temp))
		{
			r3dOutToLog("!!! Trying to load a tactic with id '%d' that is already loaded!", tacticID);
			return;
		}
	}
	uint32_t tacticType = xmlTactic.attribute("Type").as_uint();
	switch( tacticType )
	{
	case AI_TacticTypes::StraightLine:
		loadStraightLineTactic( xmlTactic, tacticID );
		break;
	case AI_TacticTypes::BezierCurve:
		loadBezierTactic( xmlTactic, tacticID );
		break;
	default:
		r3dOutToLog("!!! Unknown tactic type '%d' with id '%d'.\n", tacticType, tacticID );
		return;
	}
}

void AI_Tactics::loadBaseTactic( pugi::xml_node& xmlTactic, AI_TacticConfig& tactic )
{
	r3d_assert( !xmlTactic.empty() );

	_tcsncpy_s(tactic.m_Name, AI_TacticConfig::NameLen, xmlTactic.attribute( "Name" ).value(), AI_TacticConfig::NameLen - 1);
	r3d_assert( 0 < strlen( tactic.m_Name ) );

	// Save the tactic to the hash.
	m_tacticsHash.Add( tactic.m_TacticID, &tactic );
	m_tacticsHashKeys.push_back( tactic.m_TacticID );
}

void AI_Tactics::loadStraightLineTactic( pugi::xml_node& xmlTactic, uint32_t& tacticID )
{
	r3d_assert( !xmlTactic.empty() );

	AI_StraightLineTacticConfig* tactic = game_new AI_StraightLineTacticConfig( tacticID );

	tactic->m_TacticType = AI_TacticTypes::StraightLine;
	loadBaseTactic( xmlTactic, *tactic );
}

void AI_Tactics::loadBezierTactic( pugi::xml_node& xmlTactic, uint32_t& tacticID )
{
	r3d_assert( !xmlTactic.empty() );

	AI_BezierTacticConfig* tactic = game_new AI_BezierTacticConfig( tacticID );

	tactic->m_TacticType = AI_TacticTypes::BezierCurve;
	tactic->m_NumPoints = xmlTactic.attribute("NumPoints").as_uint();
	r3d_assert( 2 <= tactic->m_NumPoints );
	r3d_assert( 4 >= tactic->m_NumPoints ); // General Bezier curve messes up atm, so we are limited to 4 points.
	
	tactic->m_Points = game_new r3dPoint3D[tactic->m_NumPoints];
	uint32_t i = 0;
	pugi::xml_node xmlPoint = xmlTactic.child( "Point" );
	for( ; !xmlPoint.empty(); xmlPoint = xmlPoint.next_sibling(), ++i )
	{
		tactic->m_Points[ i ] = r3dPoint3D ( xmlPoint.attribute("x").as_float(),
											 xmlPoint.attribute("y").as_float(),
											 xmlPoint.attribute("z").as_float() );
	}
	r3d_assert( i == tactic->m_NumPoints );

	loadBaseTactic( xmlTactic, *tactic );
}

uint32_t AI_Tactics::GetTacticID( const char* name ) const
{
	m_tacticsHash.IterateStart();
	while( m_tacticsHash.IterateNext() )
	{
		AI_TacticConfig* tactic = m_tacticsHash.IterateGet();
		if( 0 == stricmp( tactic->m_Name, name ) )
			return tactic->m_TacticID;
	}
	r3d_assert( 0 && "invalid tactic ID!" );
	return UINT_MAX;
}

bool AI_Tactics::GetEvalTacticParams( const uint32_t& tacticID, AI_TacticParams& params ) const
{
	AI_TacticConfig* tactic = NULL;
	m_tacticsHash.GetObject( tacticID, &tactic );
	if( tactic )
		return tactic->GetEvalTacticParams( params );

	return false;
}

bool AI_Tactics::GetExecTacticParams( AI_TacticParams& params ) const
{
	// NOTE: Call GetEvalTacticParams before
	//       calling GetExecTacticParams.

	AI_TacticConfig* tactic = params.m_TacticConfig;
	if( tactic )
		return tactic->GetExecTacticParams( params );

	return false;
}

r3dPoint3D AI_Tactics::GetPosition( const AI_TacticParams& params, const float& timeSpent ) const
{
	// NOTE:	To compensate for different speeds of travel,
	//			dialate the amount of time added to your
	//			timeSpent each frame by your movement speed.

	AI_TacticConfig* tactic = params.m_TacticConfig;
	r3d_assert( tactic );

	r3dPoint3D point = tactic->GetPosition( params, timeSpent );

	// Adjust to height of ground in world
	//point = gServerLogic.AdjustPositionToFloor( point );
	gAutodeskNavMesh.AdjustNavPointHeight( point, 10.0f );
	//r3dOutToLog( "      F<%0.2f, %0.2f, %0.2f>\n", point.x, point.y, point.z );

	return point;
}

#endif