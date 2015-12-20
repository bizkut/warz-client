// HDD @ NM 16-03-07

#ifndef NM_TINYXML_FAST_HEAP
#define NM_TINYXML_FAST_HEAP

#include <new.h>
#include <malloc.h>
#include <vector>
#include <stack>

#if defined __linux__ || defined(NMTINYXML_STATIC)
# define TINYXML_PUBLIC
#else
# ifdef NMTINYXML_EXPORTS
#  define TINYXML_PUBLIC __declspec(dllexport)
# else
#  define TINYXML_PUBLIC __declspec(dllimport)
# endif
#endif

#ifdef _MSC_VER
# pragma warning( push )
# pragma warning( disable : 4100 )
#endif

/**
 * a specialized memory heap manager designed for use with TinyXML;
 * it can only allocate, deallocates do nothing. once finished with, all heaps are simply discarded.
 * it creates a heap of 'heapSize' bytes and allocates directly from that
 * pool without adding headers/footers to the returned pointers - once full, it allocates
 * new pools of at least 'subHeapSize' bytes. 
 *
 */
namespace NM
{

class TINYXML_PUBLIC TiXmlFastHeap
{
public:

  TiXmlFastHeap(size_t heapSize, size_t subHeapSize);
  ~TiXmlFastHeap();

  void* alloc   (size_t size);
  void  dealloc (void* ptr);

  template <typename _T>
  inline _T* alloc() { return (_T*)alloc(sizeof(_T)); }

  void resetForReuse();
  void logStats(const char* logTag);

protected:

  // create a new heap and assign to m_currentHeap
  void newSubHeap(size_t sz);

  // heap structure used to store everything we need to know about
  // a single storage block
  struct Heap
  {
    unsigned char*  m_heap;
    size_t          m_heapUse,
                    m_heapSize;
    unsigned int    m_pad;
  };
  typedef std::vector<Heap*> HeapVector;

  HeapVector*       m_heapList;         ///< list of used heaps
  Heap*             m_currentHeap;      ///< heap being used presently
  unsigned int      m_heapWalkIdx;      ///< 

  unsigned int      m_allocCount,       ///< number of alloc calls
                    m_deallocCount;     ///< number of dealloc calls
  size_t            m_subHeapSize,      ///< size of a new sub-heap, set in ctor
                    m_totalAllocSize;   ///< total memory usage footprint
};

extern TiXmlFastHeap* gTiXmlFastHeap;

#define TiXMemAlloc()           if (gTiXmlFastHeap) { return gTiXmlFastHeap->alloc(size); } else return malloc(size);
#define TiXMemDealloc()         if (gTiXmlFastHeap) { return gTiXmlFastHeap->dealloc(ptr); } else return free(ptr);

// every class in TinyXML now derives from this, which therefore hooks all memory allocation to use gTiXmlFastHeap
class TINYXML_PUBLIC TiXmlMemoryBase
{
public:

  void* operator new( size_t size ) { TiXMemAlloc(); }
  void* operator new( size_t size, const std::nothrow_t & e ) { TiXMemAlloc(); }
  void* operator new( size_t size, const char* filename, int line ) { TiXMemAlloc(); }
  void* operator new( size_t size, const std::nothrow_t & e, const char* filename, int line ) { TiXMemAlloc(); }

  void* operator new[]( size_t size ) { TiXMemAlloc(); }
  void* operator new[]( size_t size, const std::nothrow_t & e ) { TiXMemAlloc(); }
  void* operator new[]( size_t size, const char* filename, int line ) { TiXMemAlloc(); }
  void* operator new[]( size_t size, const std::nothrow_t & e, const char* filename, int line ) { TiXMemAlloc(); }

  void operator delete( void * ptr ) { TiXMemDealloc(); }
  void operator delete( void * ptr, const std::nothrow_t& e ) { TiXMemDealloc(); }
  void operator delete( void * ptr, const char* filename, int line ) { TiXMemDealloc(); }
  void operator delete( void * ptr, const std::nothrow_t & e, const char* filename, int line ) { TiXMemDealloc(); }

  void operator delete[]( void * ptr ) { TiXMemDealloc(); }
  void operator delete[]( void * ptr, const std::nothrow_t& e ) { TiXMemDealloc(); }
  void operator delete[]( void * ptr, const char* filename, int line ) { TiXMemDealloc(); }
  void operator delete[]( void * ptr, const std::nothrow_t& e, const char* filename, int line ) { TiXMemDealloc(); }
};

} // namespace NM

#ifdef _MSC_VER
# pragma warning( pop )
#endif

#endif // NM_TINY_XML_FAST_HEAP


