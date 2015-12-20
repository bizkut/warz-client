#pragma once

#define MAX_STRVAL_STATIC		128
#define MAX_VAR_NAME_LEN		64

#include "Tsg_stl/TString.h"


enum
{
	VF_HIDDEN		= ( 1 << 2 ),	// hidden from list of all available commands and from auto-complete feature
	VF_CONST		= ( 1 << 3 ),	// constant values can only be set during loadtime using 'set'
	VF_FORCEECHO	= ( 1 << 4 ),
	VF_NEVERDEFER	= ( 1 << 5 ),
	VF_SAVE			= ( 1 << 6 ),
	VF_CONSTRAINT	= ( 1 << 7 ),
};

typedef void (*VarChangeCallback)(int oldI, float oldF);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	class CmdVar
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class CmdVar
{
protected:

	FixedString		m_sName;
	DWORD			m_dwFlags;
	
	FixedString256	m_sVal;
	int				m_nVal;
	float			m_fVal;

	float			m_fMaxVal;
	float			m_fMinVal;

	VarChangeCallback	m_VarChangeCallback;

public:

	enum
	{
		MAX_STRING_LENGTH = FixedString256::MAX_LENGTH
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	>	SET
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void						SetString		( const char * szVal );
	void						SetInt			( int nVal );
	void						SetFloat		( float fVal );
	void						SetBool			( bool bVal );

	void						SetVal			( const char * szVal );
	void						SetVal			( int nVal );
	void						SetVal			( float fVal );


	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	>	GET
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline const char *			GetString		() const				{ return m_sVal; };
	inline int					GetInt			() const				{ return m_nVal; };
	inline float				GetFloat		() const				{ return m_fVal; };
	inline bool					GetBool			() const				{ return m_nVal != 0; };

	inline const char *			GetName			() const				{ return m_sName; }
	inline DWORD				GetFlags		() const				{ return m_dwFlags; }

	inline float				GetMinVal		() const				{ return m_fMinVal; }
	inline float				GetMaxVal		() const				{ return m_fMaxVal; }

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	>	Operators
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	const char *				operator =		( const char * szStr );
	int							operator =		( const int nVal );
	float						operator =		( const float fVal );
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	>	Constructors
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	CmdVar		( const char * szName, const char * szVal, DWORD dwFlags );
	CmdVar		( const char * szName, int nVal, DWORD dwFlags );
	CmdVar		( const char * szName, float fVal, DWORD dwFlags );
	CmdVar		( const char * szName, float fVal, float fMinVal, float fMaxVal, DWORD dwFlags );

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	>	Aux functions
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SetChangeCallback(VarChangeCallback fn);
	VarChangeCallback GetChangeCallback() const;
};
