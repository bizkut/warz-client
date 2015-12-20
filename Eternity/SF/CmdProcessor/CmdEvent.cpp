#include "r3dPCH.h"
#include "CmdEvent.h"


//---------------------------------------------------------------------------------------------
// Name: 
// Desc: One of constructor signatures
//---------------------------------------------------------------------------------------------
CEventVar::CEventVar( const char * text ) 
	: nVal(0)
	, fVal(0)
{
	assert( text );

	dwDirtyFlags = DIRTY_ALL &~ DIRTY_STRING;

	if ( ! text )
	{
		szVal[ 0 ] = '\0';
	}
	else
	{
		r3dscpy( szVal, text );
		//szVal[ sizeof( szVal ) - 1 ] = 0;
	}

	dwType = IS_STRING;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	CEventVar methods
//
///////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
// Name: 
// Desc: Gets string from the var
//---------------------------------------------------------------------------------------------
const char * CEventVar::GetString()
{
	if ( dwDirtyFlags & DIRTY_STRING )
	{
		switch( dwType )
		{
			case IS_INTEGER:
				sprintf( szVal, "%d", nVal );
				break;

			case IS_FLOAT:
				sprintf( szVal, "%f", fVal );
				break;

			default:
				r3dscpy( szVal, "unknown" );	// here should be another checks
				break;
		}

		dwDirtyFlags &= ~DIRTY_STRING;
	}

	return szVal;
}


//---------------------------------------------------------------------------------------------
// Name: 
// Desc: Gets integer from the var
//---------------------------------------------------------------------------------------------
int CEventVar::GetInteger()
{
	if ( dwDirtyFlags & DIRTY_INTEGER )
    {
		switch( dwType )
        {
			case IS_STRING :
				nVal = atoi( szVal );
				break;

			case IS_FLOAT :
				nVal = ( int ) ( fVal );
				break;

			default:
				nVal = 0;
				break;
		}

		dwDirtyFlags &= ~DIRTY_INTEGER;
    }

	return nVal;
}


//---------------------------------------------------------------------------------------------
// Name: 
// Desc: Gets float from the var
//---------------------------------------------------------------------------------------------
float CEventVar::GetFloat()
{
	if ( dwDirtyFlags & DIRTY_FLOAT )
    {
		switch( dwType )
        {
			case IS_STRING :
				fVal = (float)atof( szVal );
				break;

			case IS_INTEGER :
				fVal = ( float ) ( nVal );
				break;

			default:
				fVal = 0.0f;
				break;
		}

		dwDirtyFlags &= ~DIRTY_FLOAT;
    }

	return fVal;
}


//---------------------------------------------------------------------------------------------
// Name: 
// Desc: Gets the token from the var
//---------------------------------------------------------------------------------------------
const char * CEventVar::GetToken()
{
	if ( dwDirtyFlags & DIRTY_STRING )
	{
		switch( dwType )
		{
		
		case IS_INTEGER:
			sprintf( szVal, "%d", nVal );
			break;

		case IS_FLOAT:
			sprintf( szVal, "%f", fVal );
			break;

		default:
			r3dscpy( szVal, "unknown" );	// here should be another checks
		}

		dwDirtyFlags &= ~DIRTY_STRING;
	}

	return szVal;
}


//---------------------------------------------------------------------------------------------
// Name: 
// Desc: Converts string into array of tokens
//---------------------------------------------------------------------------------------------
void VarsContainer_c::Tokenize( const char * parmstring )
{
	static char		szToken[ 1024 ];
	const char *	ptext = parmstring;

	while((ptext)&&(*(ptext)))
	{
		char *ptok=szToken;
		while((ptext)&&(*(ptext))&&(strchr("\t\n ,", *(ptext)))) 
			ptext++;

		if (*ptext == '(')
		{
			int iBraced = 0;
			ptext++;

			while( (ptext) && (*ptext) && ( (*ptext != ')') || (iBraced != 0) ) )
			{
				if (*ptext == '(') iBraced++;
				else if (*ptext == ')') iBraced--;
				*(ptok++) = *(ptext++);				
			}

			if (*ptext == ')') ptext++;

			*ptok=0x0;
			AddString(szToken);

		} else
		{
			while((ptext) && (*(ptext)) && (!strchr("\t\n ,(", *(ptext))))
				*(ptok++)=*(ptext++);

			*ptok=0x0;
			if (szToken[0]) AddString(szToken);
		}
	}

// Disabled by RCL - fed up with laaa-aarge logs
/*
	LOG_PRINT( "Parse string:  >%s<", parmstring );
	for ( int i = 0; i < NumArgs(); i++ )
		LOG_PRINT( "token #%d is  >%s<", i, GetString( i ) );
*/
}



//---------------------------------------------------------------------------------------------
// Name: 
// Desc: Merges all the args into one string
//---------------------------------------------------------------------------------------------
const char * VarsContainer_c::GetArgsFrom( int pos, int separate ) const
{
	static char s_szArgs[ 4 ][ 16384 ];
	static int s_nArgBuf = 0;

	if ( pos < 0 || pos >= ( int ) evVars.Count() )
		return NULL;

	char * pArgBuf = s_szArgs[ s_nArgBuf ];
	s_nArgBuf ++; 
	s_nArgBuf &= 0x03;

	pArgBuf[ 0 ] = '\0';

	for ( int i = pos; i < ( int ) evVars.Count(); i++ )
	{
		if ( separate == SEP_PARENTHESIS )
		{
			strcat( pArgBuf, "(" );
		}

		strcat( pArgBuf, evVars[ i ].GetToken() );
		
		if ( separate == SEP_PARENTHESIS )
		{
			strcat( pArgBuf, ") " );
		}
		else if ( separate == SEP_SEMICOLON )
		{
			strcat( pArgBuf, "; " );
		}
		else if ( i < ( int ) evVars.Count() - 1 )
		{
			strcat( pArgBuf, " " );
		}
	}

	return pArgBuf;
}
