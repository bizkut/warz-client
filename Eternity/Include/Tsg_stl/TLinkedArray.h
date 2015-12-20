#ifndef	TL_TLINKEDARRAY_H
#define	TL_TLINKEDARRAY_H

namespace r3dTL
{
	struct TDefaultLinkedArraySettings
	{
		enum
		{
			VACANT_VALUE = -1,
			CHUNK_LENGTH = 3
		};
	};

	template < typename T, typename S = TDefaultLinkedArraySettings >
	class TLinkedArray
	{
	public:
		typedef S Settings;

		enum
		{
			CHUNK_LENGTH = Settings::CHUNK_LENGTH,
			VACANT_VALUE = Settings::VACANT_VALUE
		};

		struct ListElem
		{
			T			Elems[ CHUNK_LENGTH ];
			ListElem*	Next;

			ListElem();
		};

		class Iterator
		{
			friend class TLinkedArray;

		public:
			Iterator();

		public:
			T& GetValue();
			void SetValue( const T& val );

			void Advance();
			bool IsValid() const;
		private:
			ListElem*	mListElem;
			int			mIndex;
		};

	public:
		TLinkedArray();

		TLinkedArray( const TLinkedArray& cpy );
		
		TLinkedArray& operator =( const TLinkedArray& cpy );

		~TLinkedArray();

		void DeleteAll();

		Iterator GetIterator() const;
		Iterator GetIterator( int index ) const;

		void Insert( Iterator& iter, const T& val );
		void Erase( Iterator& iter );

		void PushBack( const T& elem );
		void ClearValues();

		bool IsEmpty() const;

		int Count() const;

		Iterator Find( const T& val );

	private:
		ListElem mListElem;
	};

	//------------------------------------------------------------------------

	template < typename T, typename S >
	TLinkedArray< T, S >::ListElem::ListElem()
	{
		for( int i = 0, e = CHUNK_LENGTH; i < e; i ++ )
		{
			Elems[ i ] = (T)VACANT_VALUE;
		}

		Next = NULL;
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	TLinkedArray< T, S >::Iterator::Iterator()
	{
		mListElem = 0;
		mIndex = 0;
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	T&
	TLinkedArray< T, S >::Iterator::GetValue()
	{
		r3d_assert( IsValid() );
		return mListElem->Elems[ mIndex ];
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	void
	TLinkedArray< T, S >::Iterator::SetValue( const T& val )
	{
		r3d_assert( mListElem->Elems );
		mListElem->Elems[ mIndex ] = val;
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	R3D_FORCEINLINE
	void TLinkedArray< T, S >::Iterator::Advance()
	{
		if( mIndex < CHUNK_LENGTH - 1 )
		{
			mIndex++;
		}
		else
		{
			mListElem = mListElem->Next;
			mIndex = 0;
		}
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	R3D_FORCEINLINE
	bool TLinkedArray< T, S >::Iterator::IsValid() const
	{
		return mListElem && mListElem->Elems[ mIndex ] != (T)VACANT_VALUE;
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	TLinkedArray< T, S >::TLinkedArray()
	{
		
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	TLinkedArray< T, S >::TLinkedArray( const TLinkedArray& cpy )
	{
		*this = cpy;
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	TLinkedArray< T, S >& TLinkedArray< T, S >::operator =( const TLinkedArray& cpy )
	{
		DeleteAll();

		for( Iterator iter = cpy.GetIterator(); iter.IsValid(); iter.Advance() )
		{
			PushBack( iter.GetValue() );
		}

		return *this;
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	TLinkedArray< T, S >::~TLinkedArray()
	{
		ListElem* next = mListElem.Next;

		for( ; next; )
		{
			ListElem* cur = next;
			next = next->Next;
			delete cur;
		}
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	void TLinkedArray< T, S >::DeleteAll()
	{
		ListElem* next = mListElem.Next;

		for( ; next; )
		{
			ListElem* cur = next;
			next = next->Next;
			delete cur;
		}

		mListElem.Next = NULL;
		ClearValues();
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	R3D_FORCEINLINE
	typename TLinkedArray< T, S >::Iterator TLinkedArray< T, S >::GetIterator() const
	{
		Iterator iter;

		iter.mListElem = const_cast< ListElem* >( &mListElem );

		return iter;
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	R3D_FORCEINLINE
	typename TLinkedArray< T, S >::Iterator TLinkedArray< T, S >::GetIterator( int index ) const
	{
		Iterator iter = GetIterator();

		for( ; index; index-- )
		{
			iter.Advance();
		}

		return iter;
	}

	//------------------------------------------------------------------------
	
	template < typename T, typename S >
	void
	TLinkedArray< T, S >::Insert( Iterator& iter, const T& val )
	{
		if( !iter.IsValid() )
		{
			if( !iter.mListElem )
			{
				ListElem* elem = &mListElem;

				for( ; ; )
				{
					if( elem->Next )
						elem = elem->Next;
					else
					{
						elem->Next = new ListElem;
						elem->Next->Elems[ 0 ] = val;
						break;
					}
				}
			}
			else
				iter.SetValue( val );

			return;
		}

		Iterator untilEnd = iter;
		Iterator prev = iter;

		for( ; ;  ) 
		{
			untilEnd.Advance();

			if( !untilEnd.IsValid() )
				break;

			prev.Advance();
		}

		if( !untilEnd.mListElem )
		{
			prev.mListElem->Next = new ListElem;
		}

		T insertVal = val;

		int movementStart = iter.mIndex;

		ListElem* ptr = iter.mListElem;

		for( ; ptr ; )
		{
			T nextInsertVal = ptr->Elems[ CHUNK_LENGTH - 1 ];

			for( int i = CHUNK_LENGTH - 1; i >= movementStart + 1; i -- )
			{
				ptr->Elems[ i ] = ptr->Elems[ i - 1 ];
			}

			ptr->Elems[ movementStart ] = insertVal;
			insertVal = nextInsertVal;

			movementStart = 0;

			ptr = ptr->Next;
		}
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	void
	TLinkedArray< T, S >::Erase( Iterator& iter )
	{
		r3d_assert( iter.IsValid() );

		ListElem* elem = iter.mListElem;

		int start = iter.mIndex;

		for( ; ; )
		{
			for( int i = start; i < CHUNK_LENGTH - 1; i ++ )
			{
				elem->Elems[ i ] = elem->Elems[ i + 1 ];
			}

			if( elem->Next )
			{
				elem->Elems[ CHUNK_LENGTH - 1 ] = elem->Next->Elems[ 0 ];
				start = 0;
				elem = elem->Next;
			}
			else
			{
				elem->Elems[ CHUNK_LENGTH - 1 ] = (T)VACANT_VALUE;
				break;
			}
		}		
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	void
	TLinkedArray< T, S >::PushBack( const T& elem )
	{
		ListElem* next = &mListElem;

		for( ; ; )
		{
			for( int i = 0; i < CHUNK_LENGTH; i ++ )
			{
				if( next->Elems[ i ] == (T)VACANT_VALUE )
				{
					next->Elems[ i ] = elem;
					return;
				}
			}

			if( next->Next == NULL )
			{
				next->Next = new ListElem;
			}

			next = next->Next;
		}
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	void
	TLinkedArray< T, S >::ClearValues()
	{
		ListElem* next = &mListElem;

		for( ; next ; )
		{
			for( int i = 0; i < CHUNK_LENGTH; i ++ )
			{
				next->Elems[ i ] = (T)VACANT_VALUE;
			}

			next = next->Next;
		}
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	bool TLinkedArray< T, S >::IsEmpty() const
	{
		return mListElem->Elems[ 0 ] == VACANT_VALUE;
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	int TLinkedArray< T, S >::Count() const
	{
		int count = 0;

		Iterator iter = GetIterator();

		for( ; iter.IsValid(); iter.Advance() )
		{
			count ++;
		}

		return count;
	}

	//------------------------------------------------------------------------

	template < typename T, typename S >
	typename TLinkedArray< T, S >::Iterator
	TLinkedArray< T, S >::Find( const T& val )
	{
		Iterator iter = GetIterator();

		for( ; iter.IsValid(); iter.Advance() )
		{
			if( iter.GetValue() == val )
				break;
		}

		return iter;
	}
}

#endif