#pragma once


//#define STATISTIC_STRING

#ifdef STATISTIC_STRING

	extern void	_AddStaticStr( const char * szPtr, size_t nSize );
	extern void	_RemoveStaticStr( const char * szPtr, size_t nSize );

	#define	SS_MEM_USAGE_CHECK_ADD_STR( x, y )		_AddStaticStr( x, y )
	#define	SS_MEM_USAGE_CHECK_REMOVE_STR( x, y )	_RemoveStaticStr( x, y )

#else	// STATISTIC_STRING
	#define	SS_MEM_USAGE_CHECK_ADD_STR( x, y )
	#define	SS_MEM_USAGE_CHECK_REMOVE_STR( x, y )
#endif	// ! STATISTIC_STRING


/// PASCAL style string template
/// Most of ingame strings are PASCAL style. The concept is
/// that we sacrifice some memory in order to avoid dynamic
/// memory allocations.
template <int iLength=128> class String_T
{
protected:
	char	sData [iLength];				///< this is actual string characters.

public:
	static const int MAX_LENGTH = iLength;	///< this string max length

public:
	/// from empty string.
	/// [obvious]
	String_T ()
	{
		sData[0] = 0;
		SS_MEM_USAGE_CHECK_ADD_STR( sData, sizeof( *this ) );
	};

	~String_T ()
	{
		SS_MEM_USAGE_CHECK_REMOVE_STR( sData, sizeof( *this ) );
	};

	/// construct string from character constant string.
	/// [obvious]
	String_T ( const char * sTxt )
	{
		assert ( sTxt );
		if ( ! sTxt )
			return;

		r3dscpy_s ( sData, iLength, sTxt );
		//sData[iLength-1] = 0;
		SS_MEM_USAGE_CHECK_ADD_STR( sData, sizeof( *this ) );
	};

	/// construct string from string.
	String_T ( const String_T & str )
	{
		if ( ! str.GetDataPtr () )
			return;

		r3dscpy_s ( sData, iLength, str.GetDataPtr () );
		//sData [iLength-1] = 0;
		SS_MEM_USAGE_CHECK_ADD_STR( sData, sizeof( *this ) );
	}

	/// construct from floating point number
	/// [obvious]
	String_T ( const float f )
	{
		assert ( f >= -FLT_MAX && f <= FLT_MAX );
		snprintf ( sData, iLength, "%f", f );
		SS_MEM_USAGE_CHECK_ADD_STR( sData, sizeof( *this ) );
	};

	/// construct form integer
	String_T ( int i )
	{
		snprintf ( sData, iLength, "%i", i );
		SS_MEM_USAGE_CHECK_ADD_STR( sData, sizeof( *this ) );
	};

	/// construct from character
	/// [obvious]
	String_T ( const char C )
	{
		sData[0] = C;
		sData[1] = 0;
		SS_MEM_USAGE_CHECK_ADD_STR( sData, sizeof( *this ) );
	};

public:
	/// return string's maximum length.
	/// [obvious]
	int GetMaxLength ( void ) const
	{
		return iLength;
	};

	/// return pointer to string data.
	/// [obvious]
	char * GetDataPtr ( void ) 
	{
		return sData;
	};

	/// return constant pointer to string data.
	/// [obvious]
	const char * GetDataPtr ( void ) const
	{
		return sData;
	}
	
	
	/// return constant pointer to string data.
	/// [obvious]
	const char * c_str ( void ) const
	{
		return sData;
	}
	
	/// return pointer to string data.
	/// [obvious]
	char * str ( void ) 
	{
		return sData;
	};

	/// (const char *) operator.
	/// [obvious]
	operator const char * ( void ) const
	{
		return sData;
	};


public:

	/// return character of specific index.
	/// [obvious]
	inline char & operator [] ( int index )
	{
		return sData[index];
	};

	/// string assignment operator.
	/// [obvious]
	inline String_T & operator = ( const char * sTxt )
	{
		if ( sTxt != NULL )
		{
			r3dscpy_s ( sData, iLength, sTxt );
			//sData[iLength-1] = 0;
		}
		else
			sData [ 0 ] = 0;

		return * this;
	};

	/// string less operator.
	bool operator < ( const String_T & rhs ) const
	{
		return strcmp( sData, rhs ) < 0;
	};

	/// string concatenation and assignment operator.
	/// [obvious]
	inline String_T & operator += ( const char * sTxt )
	{
		assert ( sTxt != NULL );

		int iLen = ( int ) strlen ( sData );

//		int iAddLen = strlen ( sTxt );
//		if ( iAddLen + iLen < iLength )
//			strncat ( sData, sTxt, iLength-1 );

		if ( iLen < iLength-1 )
		{
			r3dscpy_s ( sData+iLen, iLength-iLen, sTxt );
			//sData[iLength-1] = 0;
		}

		return *this;
	};

	/// string concatenation operator.
	/// [obvious]
	inline String_T operator + ( const char * sTxt ) const
	{
		assert ( sTxt != NULL );

		String_T<iLength> sBuf(sData);
		sBuf += sTxt;
		return sBuf;
	};

public:
	/// string comparing.
	/// [obvious]
	/// \todo [boris] we'll probably need more string comparing operators.
	inline bool operator ==  ( const char * sTxt ) const
	{
		assert ( sTxt != NULL );
		if ( sTxt == NULL )
			return false;

		return strcmp(sData,sTxt)==0;
	};

	/// comparsing of two strings.
	/// [obvious]
	inline bool operator ==  ( const String_T * pStr ) const
	{
		assert ( pStr != NULL );
		if ( pStr == NULL )
			return false;

		return strcmp(sData, pStr->GetDataPtr())==0;
	};

	/// string none-comparasing.
	/// [obvious]
	inline bool operator !=  ( const char * sTxt ) const
	{
		assert ( sTxt != NULL );
		if ( sTxt == NULL )
			return false;

		return strcmp(sData,sTxt)!=0;
	};

	/// none-comparsing of two strings.
	/// [obvious]
	inline bool operator !=  ( const String_T * pStr ) const
	{
		assert ( pStr != NULL );
		if ( pStr == NULL )
			return false;

		return strcmp(sData, pStr->GetDataPtr())!=0;
	};

public:
	/// returns current string length.
	/// [obvious]
	inline int Length ( void ) const
	{
		return strlen(sData);
	};

	/// returns true if string is empty
	/// [obvious]
	inline bool IsEmpty () const
	{
		return sData [ 0 ] == 0;
	};

	inline void SetEmpty ()
	{
		*sData = '\0';
	};

	/// returns substring from a specific string.
	/// from start to the end of string.
	String_T SubString ( int start ) const
	{
		int iCurLength = Length();

		if (start < 0) start += iCurLength;
		if (start < 0) start = 0;
		if (start > iCurLength) start = iCurLength;

		return String_T (sData+start);
	}

	/// returns substring from a specific string.
	/// from start, with specific length
	String_T SubString ( int start, int length ) const
	{
		int end; // selection end index (NOT inclusive)
		int iCurLength = Length();

		if (start < 0) start += iCurLength;
		if (start < 0) start = 0;
		if (start > iCurLength) start = iCurLength;

		end = ((length < 0) ? iCurLength : start) + length;
		if (end < 0) end = 0;
		if (end > iCurLength) end = iCurLength;
		if (end < start) end = start;

		String_T<iLength> sBuf(sData+start);
		sBuf.GetDataPtr()[end-start] = 0; 
		return sBuf;
	}

	/// converts all characters to upper register.
	/// [obvious].
	String_T & ToUpper()
	{
		char *p = sData;
		while (*p)
		{
			*p = (char) toupper(*p);
			p++;
		}
		return *this;
	}

	/// converts all characters to lower register.
	/// [obvious]
	String_T & ToLower()
	{
		char *p = sData;
		while (*p)
		{
			*p = (char) tolower(*p);
			p++;
		}
		return *this;
	}

	/// Finds the first occurense of given substring.
	/// @return Match position, or -1 if no match.
	int StrPos(const char * sNeedle, int iOffset=0) const
	{
		assert ( sNeedle != NULL );

		iOffset = Max(Min(iOffset, Length()), 0);
		const char * sHaystack = GetDataPtr();
		const char * pMatch = strstr(sHaystack+iOffset, sNeedle);
		return pMatch
			? pMatch-sHaystack
			: -1;
	}

	/// Finds the first occurense of "\r\n" or "\n" or "\r" substring.
	/// @return Match position, or -1 if no match.
	int StrLineEndPos(int iOffset=0, int * pnSize = NULL ) const
	{
		int nLen = Length();
		if ( iOffset < 0 || iOffset > nLen )
		{
			assert( false );
			return -1;
		}

		const char * sHaystack = GetDataPtr();
		for ( int i = iOffset; i < nLen; ++i )
		{
			char ch = sHaystack[ i ];
			if ( ch == '\r' || ch == '\n' )
			{
				iOffset++;
				if ( pnSize )
					*pnSize = 1;
				if ( ( iOffset < nLen ) && ( ch == '\r' ) )
				{
					if ( sHaystack[ iOffset ] == '\n' )
					{
						if ( pnSize )
							*pnSize = 2;
					}
				}
				return i - 1;
			}
		}
		return -1;
	}

	/// Finds the last occurence of given substring.
	/// @return Match position, or -1 if no match.
	int StrLastPos ( const char * sNeedle ) const
	{
		assert ( sNeedle != NULL );

		const char * pLastMatch = NULL;
		const char * pMatch = strstr ( GetDataPtr(), sNeedle );
		while ( pMatch )
		{
			pLastMatch = pMatch;
			pMatch = strstr ( pMatch+1, sNeedle );
		}

		return pLastMatch
			? pLastMatch-GetDataPtr()
			: -1;
	}

	/// Checks if this string begins with a given prefix.
	bool Begins(const char * sPrefix) const
	{
		assert ( sPrefix != NULL );
		if ( sPrefix == NULL )
			return false;

		int iPrefixLen = strlen( sPrefix );

		return ( iPrefixLen > Length() ) ? false :
			( _strnicmp( sData, sPrefix, iPrefixLen ) == 0 ) ? true : false;
	}

	/// Checks if this string ends with a given prefix.
	bool Ends(const char * sPrefix) const
	{
		assert ( sPrefix != NULL );
		if ( sPrefix == NULL )
			return false;
		
		int iPrefixLen = strlen(sPrefix), iLen = Length();
		return (iPrefixLen > iLen)
			? false
			: ( (_strnicmp(sData+iLen-iPrefixLen, sPrefix, iPrefixLen)==0)
				? true
				: false );
	}

	/// Trims whitespace from left string side.
	///	@return true if the string has been changed.
	bool LTrim ( )
	{
		int iPos = 0, iLen = Length();

		while ( (iPos<iLen) && isspace ( sData[iPos] & 255 ) ) 
			iPos++;

		if ( iPos == 0 )
			return false;

		if ( iPos<iLen ) 
		{
			memmove( sData, sData+iPos, iLen-iPos );
			sData [ iLen - iPos ] = 0;
		}
		else 
			sData[0] = '\0';

		return true;
	}

	/// Trims whitespace from right string side.
	///	@return true if the string has been changed.
	bool RTrim ( )
	{
		int iPos = Length();
		do 
		{
			--iPos;
		}
		while ( iPos >= 0 && isspace ( sData[iPos] & 255 ) );

		if ( sData [ iPos + 1 ] == '\0' )
			return false;

		sData [ iPos + 1 ] = '\0';
		return true;
	}

	/// Trims whitespace from both string sides.
	///	@return true if the string has been changed.
	bool Trim ( )
	{
		return ( LTrim() | RTrim() );
	}

	/// Replaces a given character with another given character
	/// [obvious]
	void Replace ( char sFind, char sReplace )
	{
		int iLen = Length();

		for ( int i = 0; i < iLen; ++i)
			if ( sData[i] == sFind ) sData[i] = sReplace;
	}

	/// Chops a few bytes (default is 1) off the end.
	void Chop ( int iCount=1 )
	{
		int iLen = Length ();
		iCount = Min ( Max ( iCount, 1 ), iLen );

		assert ( (iLen-iCount)>=0 );
		assert ( (iLen-iCount)<=iLength );
		sData [ iLen-iCount ] = '\0';
	}
	
	int						ToInt				() const		{ return atoi( sData ); }
	float					ToFloat				() const		{ return ( float ) atof( sData ); }

	String_T &				FromInt				( int nVal )
	{
		sprintf( sData, "%d", nVal );
		return *this;
	}

	String_T &				FromFloat			( float fVal )
	{
		sprintf( sData, "%f", fVal );
		return *this;
	}

	String_T				GetExt				() const
	{
		String_T ext;
		_splitpath ( sData, NULL, NULL, NULL, ext.str() );
		return ext;
	}

	String_T				GetName				() const
	{
		String_T name;
		_splitpath ( sData, NULL, NULL, name.str(), NULL );
		return name;
	}


};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/// this is default string type of 255 characters max.
typedef String_T<32> FixedString32;
typedef String_T<128> FixedString;
typedef String_T<256> FixedString256;

/// Constructs string from format string and parameters.
template< int iLength > String_T< iLength > 
inline NewString( const char * sFmt, ... )
{
	assert ( sFmt );

	String_T< iLength > sRes;

	va_list ap;
	va_start( ap, sFmt );
	_vsnprintf( sRes.GetDataPtr(), sRes.GetMaxLength(), sFmt, ap );
	va_end( ap );

	return sRes;
}

FixedString NewString( const char * sFmt, ... );
