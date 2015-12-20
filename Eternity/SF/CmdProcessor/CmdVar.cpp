#include "r3dPCH.h"

#include "CmdVar.h"

namespace
{
	template<class T>
	__forceinline T R3D_MAX(const T a, const T b) { return a > b ? a : b; }

	template<class T>
	__forceinline T R3D_MIN(const T a, const T b)	{ return a < b ? a : b; }
}

//--------------------------------------------------------------------------------------------------------
CmdVar::CmdVar( const char * szName, const char * szVal, DWORD dwFlags )
	: m_sName( szName )
	, m_dwFlags( dwFlags )
	, m_fMinVal( 0.f )
	, m_fMaxVal( 0.f )
	, m_VarChangeCallback( 0 )
{
	SetString( szVal );
}

//--------------------------------------------------------------------------------------------------------
CmdVar::CmdVar( const char * szName, int nVal, DWORD dwFlags )
	: m_sName( szName )
	, m_dwFlags( dwFlags )
	, m_fMinVal( 0.f )
	, m_fMaxVal( 0.f )
	, m_VarChangeCallback( 0 )
{
	SetInt( nVal );
}

//------------------------------------------------------------------------

CmdVar::CmdVar( const char * szName, float fVal, DWORD dwFlags )
: m_sName( szName )
, m_dwFlags( dwFlags )
, m_fMinVal( 0.f )
, m_fMaxVal( 0.f )
, m_VarChangeCallback( 0 )
{
	SetFloat( fVal );
}

//--------------------------------------------------------------------------------------------------------

CmdVar::CmdVar( const char * szName, float fVal, float fMinVal, float fMaxVal, DWORD dwFlags )
	: m_sName( szName )
	, m_dwFlags( dwFlags )
	, m_fMinVal( fMinVal )
	, m_fMaxVal( fMaxVal )
	, m_VarChangeCallback( 0 )
{
	// sometimes ints get here and our perfectionism suffers( string is "wrong" )
	if( fabs( fVal - floorf( fVal ) ) < FLT_EPSILON )
	{
		SetInt( (int) fVal );
	}
	else
	{
		SetFloat( fVal );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	SET
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
	
//--------------------------------------------------------------------------------------------------------
void CmdVar::SetString( const char * szVal )
{
	m_sVal = szVal;

	float fVal = m_sVal.ToFloat();

	if( m_dwFlags & VF_CONSTRAINT )
	{
		// assume numeric value if VF_CONSTRAINTS is on
		SetFloat( fVal );
	}
	else
	{
		int oldI = m_nVal;
		float oldF = m_fVal;

		m_nVal = (int)m_sVal.ToInt();
		m_fVal = fVal;
		if (m_VarChangeCallback)
		{
			//	If values not equal, or this is string value
			if ((m_fVal != oldF) || (m_fVal == 0 && oldF == 0 && szVal[0]!='\0'))
			{
				m_VarChangeCallback(oldI, oldF);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------
void CmdVar::SetInt( int nVal )
{
	if( m_dwFlags & VF_CONSTRAINT )
	{
		nVal = R3D_MAX( R3D_MIN( nVal, (int)m_fMaxVal ), (int)m_fMinVal );
	}

	m_sVal.FromInt( nVal );
	int oldI = m_nVal;
	float oldF = m_fVal;
	m_nVal = nVal;
	m_fVal = ( float ) nVal;
	if (m_VarChangeCallback && oldI != m_nVal)
	{
		m_VarChangeCallback(oldI, oldF);
	}
}

//--------------------------------------------------------------------------------------------------------
void CmdVar::SetFloat( float fVal )
{
	if( m_dwFlags & VF_CONSTRAINT )
	{
		fVal = R3D_MAX( R3D_MIN( fVal, m_fMaxVal ), m_fMinVal );
	}

	int oldI = m_nVal;
	float oldF = m_fVal;

	m_nVal = ( int ) fVal;
	m_fVal = fVal;

	// sometimes ints get here and our perfectionism suffers( string is "wrong" )
	if( fabs( fVal - floorf( fVal ) ) < FLT_EPSILON )
	{
		m_sVal.FromInt( m_nVal );
	}
	else
	{
		m_sVal.FromFloat( fVal );
	}
	if (m_VarChangeCallback && oldF != m_fVal)
	{
		m_VarChangeCallback(oldI, oldF);
	}
}

//--------------------------------------------------------------------------------------------------------
void CmdVar::SetBool( bool bVal )
{
	SetInt( bVal ? 1 : 0 );
}

//--------------------------------------------------------------------------------------------------------
void CmdVar::SetVal( const char * szVal )
{ 
	SetString( szVal ); 
}

//--------------------------------------------------------------------------------------------------------
void CmdVar::SetVal( int nVal )	
{ 
	SetInt( nVal ); 
}

//--------------------------------------------------------------------------------------------------------
void CmdVar::SetVal( float fVal )			
{ 
	SetFloat( fVal ); 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	Operators
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------------
const char * CmdVar::operator = ( const char * szStr )
{
	SetString( szStr );
	return szStr;
}

//--------------------------------------------------------------------------------------------------------
int CmdVar::operator = ( const int nVal )
{
	SetInt( nVal );
	return nVal;
}

//--------------------------------------------------------------------------------------------------------
float CmdVar::operator = ( const float fVal )
{
	SetFloat( fVal );
	return fVal;
}

//--------------------------------------------------------------------------------------------------------
void CmdVar::SetChangeCallback(VarChangeCallback fn)
{
	m_VarChangeCallback = fn;
}

//--------------------------------------------------------------------------------------------------------
VarChangeCallback CmdVar::GetChangeCallback() const
{
	return m_VarChangeCallback;
}