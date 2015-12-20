#pragma once

#ifndef USE_VMPROTECT
  //@uncomment for VMProtect testing
  //#define USE_VMPROTECT 1 
#endif

// always use VMProtect in final build
#ifdef FINAL_BUILD
  #undef USE_VMPROTECT
  //#define USE_VMPROTECT 1
#endif

// always not use VMProtect in server
#ifdef WO_SERVER
  #undef USE_VMPROTECT
#endif


#if USE_VMPROTECT
  #include "..\..\..\External\VMProtect\VMProtectSDK.h"
  #define VMPROTECT_BeginMutation(XX) VMProtectBeginMutation(XX)
  #define VMPROTECT_BeginVirtualization(XX) VMProtectBeginVirtualization(XX)
  #define VMPROTECT_End() __asm {nop} VMProtectEnd()
  #define VMPROTECT_DecryptStringA(XX) VMProtectDecryptStringA(XX)
#else
  #define VMPROTECT_BeginMutation(XX)
  #define VMPROTECT_BeginVirtualization(XX)
  #define VMPROTECT_End()
  #define VMPROTECT_DecryptStringA(XX) XX
#endif

//
// class used to store protected dword-sized types with crypt key, so it can't be found directly in memory
//
#pragma pack(push)
#pragma pack(1)
template<class TYPE, DWORD CRYPT_KEY> class r3dSec_type
{
  private:
	union datamix_u {
		TYPE	val;
		struct {
		  BYTE	b1;
		  BYTE	b2;
		  BYTE	b3;
		  BYTE	b4;
		} crypt;
	} data;
#ifdef _DEBUG
	TYPE		uncrypted;
#endif

  public:	
	r3dSec_type() {
		// default consturctor, do nothing.
	}
	
	__forceinline r3dSec_type(const TYPE rhs) {
		set(rhs);
	}

	__forceinline TYPE get() const {
		datamix_u t = data;
		t.crypt.b1 ^= ((CRYPT_KEY >> 16) & 0xFF);
		t.crypt.b3 ^= ((CRYPT_KEY >>  0) & 0xFF);
		t.crypt.b2 ^= ((CRYPT_KEY >>  8) & 0xFF);
		t.crypt.b4 ^= ((CRYPT_KEY >> 24) & 0xFF);
		return t.val;
	}
	__forceinline r3dSec_type& set(const TYPE rhs) {
		data.val = rhs;
		data.crypt.b1 ^= ((CRYPT_KEY >> 16) & 0xFF);
		data.crypt.b3 ^= ((CRYPT_KEY >>  0) & 0xFF);
		data.crypt.b2 ^= ((CRYPT_KEY >>  8) & 0xFF);
		data.crypt.b4 ^= ((CRYPT_KEY >> 24) & 0xFF);
#ifdef _DEBUG		
		uncrypted = rhs;
#endif		
		return *this;
	}
	
	// accessors and setters
	__forceinline operator TYPE() const {
		return get();
	}
	__forceinline TYPE operator ->() {
		return get();
	}
	__forceinline const TYPE operator ->() const {
		return get();
	}
	__forceinline r3dSec_type& operator=(const TYPE rhs) {
		return set(rhs);
	}
};
#pragma pack(pop)

//
// class used to store protected strings
//
template<int STRING_LEN, BYTE CRYPT_KEY> class r3dSec_string
{
  private:
	char		crypted[STRING_LEN];
	int		length;
#ifdef _DEBUG		
	char		uncrypted[STRING_LEN];
#endif

  public:	
	r3dSec_string() {
		length       = 0;
		crypted[0]   = 0;
#ifdef _DEBUG		
		uncrypted[0] = 0;
#endif
	}
	
	void get(char *out) const {
		memcpy(out, crypted, length+1);
		for(int i=0; i<length; ++i) {
			out[i] = out[i] ^ ((CRYPT_KEY + i) & 0xFF);
		}
		return;
	}
	void set(const char* in) {
		length = strlen(in);
		if(length + 1 > STRING_LEN)
			r3dError("failed to set string %s\n", in); 
		memcpy(crypted, in, length + 1);
		for(int i=0; i<length; ++i) {
			crypted[i] = crypted[i] ^ ((CRYPT_KEY + i) & 0xFF);
		}
#ifdef _DEBUG		
		r3dscpy(uncrypted, in);
#endif		
	}
};

