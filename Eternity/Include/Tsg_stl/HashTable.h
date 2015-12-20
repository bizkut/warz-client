#pragma once

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	Hash table (direct addressing)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
template < typename Type, typename Key, typename HashFunc, int nSize >
class HashTableDynamic
{
	enum
	{
		eHashMask = nSize - 1
	};

	class Node_c
	{
		Node_c &	operator = ( const Node_c & n )		{ return *this; };	// no copying is allowed

	public:
	
		Type		m_tData;		// pointer to templated data
		Key			m_tKey;			// key
		Node_c *	m_pNext;

		Node_c( const Key &key, const Type & val, Node_c * pNext )
			: m_pNext( pNext )
			, m_tKey( key )
			, m_tData( val )
		{
		};
	};

protected:

	// holds array of pointers to linked chains
	Node_c **		m_pNodes;

	// elements count
	DWORD		m_dwCount;

    inline Node_c *		Search			( const Key &key ) const;
	inline Node_c *		Search			( const Key &key, DWORD &dwHash ) const;
	inline DWORD		GetHashValue	( const Key &key ) const;
	
public:
  
	
	inline bool			Add				( const Key &key, const Type &val );
	inline bool			AddAndReplace	( const Key &key, const Type &val );
	inline bool			Remove			( const Key &key );
	

	inline DWORD		Size			() const						{ return m_dwCount; }
	inline bool			IsExists		( const Key &key ) const		{ return Search( key ) != NULL; }
	inline bool			IsEmpty			() const						{ return m_dwCount == 0; }
	void				Clear			();
    
	inline Type			GetObject		( const Key &key, const Type &result = (Type ) NULL ) const;
	inline const Type *	GetObjectPtr	( const Key &key ) const;
    inline bool			GetObject		( const Key &key, Type * pResult ) const;

	HashTableDynamic();
	virtual ~HashTableDynamic();

protected:
	mutable Node_c *	m_pIterNode;
	mutable DWORD		m_dwIterHashIndex;


	void				IterNext() const
	{
		if ( m_pIterNode ) 
		{
			m_pIterNode = m_pIterNode->m_pNext;
			if ( m_pIterNode )
				return;

			m_dwIterHashIndex++;
		}

		while ( m_dwIterHashIndex < nSize )
		{	
			if ( ! m_pNodes[ m_dwIterHashIndex ] )
			{
				m_dwIterHashIndex++;
				continue;
			}

			m_pIterNode = m_pNodes[ m_dwIterHashIndex ];
			return;
		}
		return;
	}

public:

	// you have to call IterateNext right after that to populate first Node
	void IterateStart() const
	{
		m_pIterNode = NULL;
		m_dwIterHashIndex = 0;
	}

	/// do iteration
	bool IterateNext() const
	{
		IterNext();
		return m_pIterNode != NULL;
	}

	Type & IterateGet ()
	{
		assert( m_pIterNode );
		return m_pIterNode->m_tData;
	}

	const Type & IterateGet () const
	{
		assert( m_pIterNode );
		return m_pIterNode->m_tData;
	}

	const Key & IterateGetKey () const
	{
		assert( m_pIterNode );
		return m_pIterNode->m_tKey;
	}
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
HashTableDynamic< Type, Key, HashFunc, nSize >::HashTableDynamic()
	: m_dwCount( 0 )
	, m_pIterNode( NULL )
{
	m_pNodes = game_new Node_c * [ nSize ];
	memset( m_pNodes, 0, nSize * sizeof( Node_c * ) );
}

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
HashTableDynamic< Type, Key, HashFunc, nSize >::~HashTableDynamic()
{
	Clear();
	SAFE_DELETE_ARRAY( m_pNodes );
}

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
inline DWORD HashTableDynamic< Type, Key, HashFunc, nSize >::GetHashValue( const Key &key ) const
{
	return ( HashFunc() ( key ) ) & eHashMask;
}

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
inline typename HashTableDynamic< Type, Key, HashFunc, nSize >::Node_c * HashTableDynamic< Type, Key, HashFunc, nSize >::Search( const Key &key ) const
{
	DWORD dwIndex = GetHashValue( key );

	if ( m_pNodes[ dwIndex ] )
	{
		Node_c * pCurrent = m_pNodes[ dwIndex ];

		while ( pCurrent )
		{
			if ( pCurrent->m_tKey == key )
				return pCurrent;

			pCurrent = pCurrent->m_pNext;
		}
	}
	return NULL;
}

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
inline typename HashTableDynamic< Type, Key, HashFunc, nSize >::Node_c * HashTableDynamic< Type, Key, HashFunc, nSize >::Search( const Key &key, DWORD &dwHash ) const
{
	dwHash = GetHashValue( key );

	if ( m_pNodes[ dwHash ] )
	{
		Node_c * pCurrent = m_pNodes[ dwHash ];

		while ( pCurrent )
		{
			if ( pCurrent->m_tKey == key )
				return pCurrent;

			pCurrent = pCurrent->m_pNext;
		}
	}
	return NULL;
}

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
void HashTableDynamic< Type, Key, HashFunc, nSize >::Clear()
{
	for ( DWORD i = 0; i < nSize; i++ )
	{
		if ( ! m_pNodes[ i ] )
			continue;

        // erase linked chain
        Node_c * pStart = m_pNodes[ i ];
        Node_c * pCurrent = pStart;

        m_pNodes[ i ] = NULL;

        // destroy list
        while ( pStart )
        {
            pCurrent = pStart->m_pNext;
            
            SAFE_DELETE( pStart );
            pStart = pCurrent;
		}
	}

	m_dwCount = 0;
}

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
inline bool HashTableDynamic< Type, Key, HashFunc, nSize >::Add( const Key &key, const Type &val )
{	
	DWORD dwHash;

	if ( Search( key, dwHash ) )
		return false;

    Node_c * pNewNode = game_new Node_c( key, val, m_pNodes[ dwHash ] );
	m_pNodes[ dwHash ] = pNewNode;
	m_dwCount++; 

	return true;
}

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
inline bool HashTableDynamic< Type, Key, HashFunc, nSize >::AddAndReplace( const Key &key, const Type &val )
{
	DWORD dwHash;
	Node_c * pNode = Search( key, dwHash );
	if ( pNode )
	{
		pNode->m_tData = val;
		return true;
	}

	Node_c * pNewNode = game_new Node_c( key, val, m_pNodes[ dwHash ] );
	m_pNodes[ dwHash ] = pNewNode;
	m_dwCount++; 

	return true;
}

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
inline bool HashTableDynamic< Type, Key, HashFunc, nSize >::Remove( const Key &key )
{
	DWORD dwIndex = GetHashValue( key );

    if ( m_pNodes[ dwIndex ] )
    {
        Node_c * pCurrent = m_pNodes[ dwIndex ];
        Node_c * pPrev = NULL;

        while ( pCurrent )
        {
            if ( pCurrent->m_tKey == key )
            {
                // delete node
                if ( pCurrent == m_pNodes[ dwIndex ] )
					m_pNodes[ dwIndex ] = pCurrent->m_pNext;
                else
					pPrev->m_pNext = pCurrent->m_pNext;
	
                SAFE_DELETE( pCurrent );
				m_dwCount--;
				return true;
            }
            
			pPrev = pCurrent;
            pCurrent = pCurrent->m_pNext;
        }
    }

	return false;
}

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
inline Type HashTableDynamic< Type, Key, HashFunc, nSize >::GetObject( const Key &key, const Type &result ) const
{
	Node_c * pFound = Search( key );

	return pFound ? pFound->m_tData : result;
}

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
inline const Type * HashTableDynamic< Type, Key, HashFunc, nSize >::GetObjectPtr( const Key &key ) const
{
	Node_c * pFound = Search( key );

	return pFound ? &pFound->m_tData : NULL;
}

//--------------------------------------------------------------------------------------------------------
template < typename Type, typename Key, typename HashFunc, int nSize >
inline bool HashTableDynamic< Type, Key, HashFunc, nSize >::GetObject( const Key &key, Type * pResult ) const
{
	assert( pResult );
	if ( ! pResult )
		return false;
	
    Node_c * pFound = Search( key );
    if ( ! pFound )
        return false;

    *pResult = pFound->m_tData;
	return true;
}
