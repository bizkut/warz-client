#pragma once

#include "Tsg_stl/TArray.h"

#define EVENT_DEFAULT_ARGUMENTS			2

// possible var types
typedef enum
{
   IS_STRING,
   IS_BOOLEAN,
   IS_INTEGER,
   IS_FLOAT,
   IS_VECTOR
} vartype;

// dirty flags
#define DIRTY_STRING    ( 1 << 0 )
#define DIRTY_VECTOR    ( 1 << 1 )
#define DIRTY_INTEGER   ( 1 << 2 )
#define DIRTY_FLOAT     ( 1 << 3 )

#define DIRTY_ALL       ( 0x7fff )

#define EVENT_VAL_SIZE	255

///////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	event variable
//
///////////////////////////////////////////////////////////////////////////////////////////////
class CEventVar
{
private:
	short	dwType;
	short	dwDirtyFlags;
	char	szVal[ EVENT_VAL_SIZE ];
	int		nVal;
	float	fVal;
	//Vec3	vVal;

public:
	CEventVar()	: dwType(IS_INTEGER), dwDirtyFlags(DIRTY_ALL), nVal(0), fVal(0) { szVal[0] = 0; }
	~CEventVar() {}
	CEventVar( const CEventVar& ev ) 
	{
		dwType = ev.dwType;
		dwDirtyFlags = ev.dwDirtyFlags;
		nVal = ev.nVal;
		fVal = ev.fVal;
		r3dscpy( szVal, ev.szVal );
		//szVal[ sizeof( szVal ) - 1 ] = 0;
	//	vVal = ev.vVal;
	};

	CEventVar( const char * text );
		                 
	CEventVar( int val )
	{
            dwType			= IS_INTEGER;
            dwDirtyFlags	= DIRTY_ALL & ~DIRTY_INTEGER;
            nVal			= val;
			szVal[0]		= 0;
    };
		                  
	CEventVar( float val )
    {		
			dwType			= IS_FLOAT;
            dwDirtyFlags	= DIRTY_ALL & ~DIRTY_FLOAT;
            fVal			= val;
			szVal[0]		= 0;
    };

	/*CEventVar( const Vec3& vec )
	{
		dwType        = IS_VECTOR;
		dwDirtyFlags  = DIRTY_ALL & ~DIRTY_VECTOR;
		vVal = vec;
		szVal[0] = 0;
	}*/

public:
	
	int				GetInteger	();
	float			GetFloat	();
	const char *	GetString	();
	const char *	GetToken	();
//	Vec3			GetVector	();

	bool			IsVector() const	{ return dwType == IS_VECTOR; }
	bool			IsString() const	{ return dwType == IS_STRING; }
	bool			IsInteger() const	{ return dwType == IS_INTEGER; }
	bool			IsFloat() const		{ return dwType == IS_FLOAT; }
	short			Type() const		{ return dwType; };

	
	void operator = ( const CEventVar& ev )
	{
		dwType=ev.dwType;
		dwDirtyFlags=ev.dwDirtyFlags;
		r3dscpy( szVal, ev.szVal );
		//szVal[ sizeof( szVal ) - 1 ] = 0;
		nVal=ev.nVal;
		fVal=ev.fVal;
	}

	bool operator == ( const CEventVar& ev ) const
	{
		if ( dwType != ev.dwType ) 
			return false;
		
		switch( dwType )
		{
			case IS_INTEGER:
				return ( nVal == ev.nVal );

			case IS_FLOAT:
				return ( fVal == ev.fVal );

			case IS_STRING:
				return ( ! strncmp( szVal, ev.szVal, sizeof( szVal ) ) );

		//	case IS_VECTOR:
		//		return ( vVal == ev.vVal );

			default:
				return false;
		}
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	Container event variables with interface
//
///////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_EVENT_VARS		16


enum
{
	SEP_NONE,
	SEP_SEMICOLON,
	SEP_PARENTHESIS
};


class VarsContainer_c
{
	// EventVar array
	typedef r3dTL::TArray< CEventVar >	TEventVars;

public:
	mutable TEventVars	evVars;

	VarsContainer_c( const VarsContainer_c& rObj ) : evVars( rObj.evVars ) {}
	VarsContainer_c() : evVars() {}

	void	Tokenize		( const char *parmstring );
	int		NumArgs			() const			{ return evVars.Count(); };
	void	ClearParams		()					{ evVars.Clear(); }

	///////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	>	add / get
	//
	///////////////////////////////////////////////////////////////////////////////////////////////

	//---------------------------------------------------------------------------------------------
	// NAME: 
	// Description:	
	//---------------------------------------------------------------------------------------------
	inline void AddString( const char * text )
	{
		CEventVar ev( text );
		evVars.PushBack( ev );
	}


	//---------------------------------------------------------------------------------------------
	// NAME: 
	// Description:	
	//---------------------------------------------------------------------------------------------
	inline void AddObject( const int val )
	{
		CEventVar ev( val );
		evVars.PushBack( ev );
	}


	//---------------------------------------------------------------------------------------------
	// NAME: 
	// Description:	
	//---------------------------------------------------------------------------------------------
	inline void AddInteger( const int val )
	{
		CEventVar ev( val );
		evVars.PushBack( ev );
	}


	//---------------------------------------------------------------------------------------------
	// NAME: 
	// Description:	
	//---------------------------------------------------------------------------------------------
	inline void AddFloat( const float val )
	{
		CEventVar ev( val );
		evVars.PushBack( ev );
	}
	
	inline void DelVar(const int pos )
	{
		assert( ( pos >= 0 ) && ( pos < ( int ) evVars.Count() ) );
		evVars.Erase(pos);
	}

	//---------------------------------------------------------------------------------------------
	// NAME: 
	// Description:	
	//---------------------------------------------------------------------------------------------
	inline const char * GetString( int pos ) const
	{
		assert ( ( pos >= 0 ) && ( pos < ( int ) evVars.Count() ) );

		return evVars[ pos ].GetToken();
	}


	//---------------------------------------------------------------------------------------------
	// NAME: 
	// Description:	
	//---------------------------------------------------------------------------------------------
	inline int GetInteger( int pos ) const
	{
		assert ( ( pos >= 0 ) && ( pos < ( int ) evVars.Count() ) );
   
		return evVars[ pos ].GetInteger();
	}


	//---------------------------------------------------------------------------------------------
	// NAME: 
	// Description:	
	//---------------------------------------------------------------------------------------------
	inline float GetFloat( int pos ) const
	{
		assert ( ( pos >= 0 ) && ( pos < ( int ) evVars.Count() ) );
   
		return evVars[ pos ].GetFloat();
	}


	//---------------------------------------------------------------------------------------------
	// NAME: 
	// Description:	
	//---------------------------------------------------------------------------------------------
	inline bool GetBoolean( int pos ) const
	{
		assert ( ( pos >= 0 ) && ( pos < ( int ) evVars.Count() ) );

		return ( GetInteger( pos ) != 0 ) ? true : false;
	}

	
	//---------------------------------------------------------------------------------------------
	// NAME: 
	// Description:	
	//---------------------------------------------------------------------------------------------
	inline void GetToken( char * buffer, int size, int pos = 0 ) const
	{
		assert ( ( pos >= 0 ) && ( pos < ( int ) evVars.Count() && buffer != NULL ) );
		
		buffer[ 0 ] = '\0';
		for ( int i = pos; i < ( int ) evVars.Count(); i++ )
		{
			strcat( buffer, evVars[ i ].GetString() );
			if ( i < ( int ) evVars.Count() - 1 )
				strcat( buffer, " " );
		}
		return;
	}


	const char * GetArgsFrom( int pos, int separate = SEP_NONE ) const;


};

