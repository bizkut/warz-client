#ifndef TL_FIXEDALLOCATOR_H
#define TL_FIXEDALLOCATOR_H

// need to undef new, as here we are using placement new which isn't compatible with memory tracking :( but it still will track malloc :)
#ifdef _DEBUG
#ifdef ENABLE_MEMORY_DEBUG
#undef new
#endif //ENABLE_MEMORY_DEBUG
#endif

namespace r3dTL
{
	template<typename T>
	class TFixedAllocator
	{
	public:
		template <class U> friend class TFixedAllocator;


		//    typedefs
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;

		//    convert an allocator<T> to allocator<U>
		template<typename U>
		struct rebind 
		{
			typedef TFixedAllocator<U> other;
		};

		inline explicit TFixedAllocator( size_t maxItems )
		: mArea( (T*)malloc( maxItems * sizeof(T) ) )
		, mMaxItems( maxItems )
		, mPtr( 0 )
		{
		}

		inline explicit TFixedAllocator(TFixedAllocator const& cpy )
		: mArea( (T*)malloc( cpy.mMaxItems * sizeof(T) ) )
		, mMaxItems( cpy.mMaxItems )
		, mPtr( 0 )
		{

		}

		template<typename U>
		inline explicit TFixedAllocator(TFixedAllocator<U> const& cpy )
		: mArea( (T*)malloc( cpy.mMaxItems * sizeof(T) ) )
		, mMaxItems( cpy.mMaxItems )
		, mPtr( 0 )
		{
			
		}

		inline ~TFixedAllocator()
		{
			free( mArea );
		}

		//    address
		inline pointer address(reference r) { return &r; }
		inline const_pointer address(const_reference r) { return &r; }

		//    memory allocation
		inline pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0) 
		{
			pointer r = reinterpret_cast<pointer>( mArea + mPtr ) ;

			mPtr += cnt;
			r3d_assert( mPtr <= mMaxItems );

			return r;
		}
		inline void deallocate(pointer p, size_type)
		{ 

		}

		//    size
		inline size_type max_size() const 
		{
			return 0xffffffff;
		}

		//    construction/destruction

		inline void construct(pointer p, const T& t) 
		{
			new( p ) T( t );
		}

		inline void destroy(pointer p) { p->~T(); }

		inline bool operator==( TFixedAllocator const& ) { return true; }
		inline bool operator!=( TFixedAllocator const& a ) { return !operator==(a); }

	private:
		T				*mArea ;
		size_t			mPtr ;
		size_t			mMaxItems ;
	};
}

#endif