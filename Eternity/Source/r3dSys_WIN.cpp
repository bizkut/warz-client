#include "r3dPCH.h"
#include "r3d.h"

#define DEFAULT_SEED	161803398l

struct URandomState
{
	unsigned long 	table[55];
	size_t 		index1;
	size_t 		index2;
};

class URandomGenerator
{
protected:
	static __declspec(thread) URandomState* State;

public:

	static unsigned long Rand(unsigned long limit)
	{
		if(limit == 0)
			return 0;

		URandomState* s = State ;

		size_t index1 = s->index1;
		size_t index2 = s->index2;

		index1 = (index1 + 1) % 55;
		index2 = (index2 + 1) % 55;

		size_t val1 = s->table[index1] - s->table[index2];

		s->index1 = index1;
		s->index2 = index2;

		s->table[index1] = val1;

		return val1 % limit;
	}

	static void InitSeed( unsigned long j );

	static void Init();
	static void	Close();
};

/*static*/
__declspec(thread) URandomState* URandomGenerator::State = NULL ;

enum
{
	MAX_RANDOM_STATES = 128
};

static URandomState RandStates[ MAX_RANDOM_STATES ];
static bool Allocated[ MAX_RANDOM_STATES ];
static LONG volatile InAllocation;

void URandomGenerator::InitSeed(unsigned long j)
{
 
  unsigned long k = 1;

  State->table[54] = j;
  for(int i = 0; i < 54; i++) {
    size_t ii = 21 * i % 55;
    State->table[ii] = k;
    k = j - k;
    j = State->table[ii];
  }
  for(int loop = 0; loop < 4; loop++) {
    for(int i = 0; i < 55; i++)
      State->table[i] = State->table[i] - State->table[(1 + i + 30) % 55];
  }
  State->index1 = 0;
  State->index2 = 31;
}

void URandomGenerator::Init()
{
	r3d_assert( !State );

	for ( ; InterlockedExchange( &InAllocation, 1 ) ; );

	for( size_t i = 0, e = R3D_ARRAYSIZE(Allocated); i < e; i ++ )
	{
		if( !Allocated[ i ] )
		{
			State = &RandStates[ i ];
			Allocated[ i ] = true ;
			break;
		}
	}

	InAllocation = 0;

	InitSeed( DEFAULT_SEED );
}


void URandomGenerator::Close()
{
	r3d_assert( State );

	for ( ; InterlockedExchange( &InAllocation, 1 ) ; );

	Allocated[ State - RandStates ] = false ;

	InAllocation = 0;
}

int random(int limit)
{
  r3d_assert(limit>=0);
  return URandomGenerator::Rand( limit );
}

unsigned long u_random(unsigned long limit)
{
  return URandomGenerator::Rand(limit);
}

void u_srand(unsigned long j)
{
	URandomGenerator::InitSeed(j);
}

float u_GetRandom()
{
  return (float)u_random(65000) / 64999.0f;
}

float u_GetRandom(float r1, float r2)
{
  return r1 + (u_GetRandom() * (r2-r1));
}

int u_GetFileTime(char *fname, FILETIME *tim)
{
	HANDLE	h;
  h = CreateFile(
    fname, 
    GENERIC_READ, 
    0, 
    NULL, 
    OPEN_EXISTING, 
    FILE_ATTRIBUTE_NORMAL, 
    NULL);
  if(h == INVALID_HANDLE_VALUE) {
    //sgLog("GetFileTime %s failed 1 :%d/%d\n", fname, GetLastError(), GetLastError());
    return 0;
  }

  if(GetFileTime(h, NULL, NULL, tim) == 0) {
    //sgLog("GetFileTime %s failed\n", fname);
    ;
  }
  CloseHandle(h);
  return 1;
}

int u_SetFileTime(char *fname, FILETIME *tim)
{
	HANDLE	h;

  h = CreateFile(
    fname, 
    GENERIC_READ | GENERIC_WRITE, 
    0, 
    NULL, 
    OPEN_EXISTING, 
    FILE_ATTRIBUTE_NORMAL, 
    NULL);
  if(h == INVALID_HANDLE_VALUE) {
    //sgLog("SetFileTime %s failed 1\n", fname);
    return 0;
  }

  if(SetFileTime(h, tim, tim, tim) == 0) {
    //sgLog("SetFileTime %s failed\n", fname);
    ;
  }
  CloseHandle(h);
  return 1;
}

void u_thread_rand_init()
{
	URandomGenerator::Init();
}

void u_thread_rand_close()
{
	URandomGenerator::Close();
}

struct DefaultThreadRandInit
{
	DefaultThreadRandInit()
	{
		u_thread_rand_init();
	}

	~DefaultThreadRandInit()
	{
		u_thread_rand_close();
	}
} static gDefaultThreadRandInit;
