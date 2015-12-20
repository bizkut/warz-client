#include "r3dPCH.h"
#include "r3d.h"

class r3dIniFileReader
{
  protected:
	enum { 
	  MAX_INI_SIZE  = 131072, 
	};
	char*		buf_;
	int		len_;
	char		curIni_[MAX_PATH];
	bool		goodIniFile_;
	
	void		Init();
	bool		ValidateFileName(const char* lpFileName);
	void		Preprocess();
	const char*	GetNextLine(const char* line);
	
  public:
	r3dIniFileReader();
	~r3dIniFileReader();

	void		InvalidateFileName ();
	
	bool		GetPrivateProfileString(const char* lpAppName, const char* lpKeyName, 
			  const char* lpDefault, char* lpReturnedString, int nSize, 
	                  const char* lpFileName);
};

r3dIniFileReader _r3d_iniReader;

r3dIniFileReader::r3dIniFileReader()
{
  buf_ = NULL;
  curIni_[0] = 0;
}

r3dIniFileReader::~r3dIniFileReader()
{
  //SAFE_DELETE(buf_);
}

void r3dIniFileReader::Init()
{
  if(buf_ != NULL)
    return;
    
  buf_ = (char *)malloc(MAX_INI_SIZE);
  if(buf_ == NULL)
	  r3dError("Out of memory!");

  curIni_[0] = 0;
  return;
}

bool r3dIniFileReader::ValidateFileName(const char* lpFileName)
{
  if(_stricmp(curIni_, lpFileName) == NULL) {
    return goodIniFile_;
  }
  
  r3dscpy(curIni_, lpFileName);
  goodIniFile_ = false;
  
  r3dFile* f = r3d_open(lpFileName, "rt");
  if(!f) {
    r3dOutToLog("ini: no ini file %s\n", lpFileName);
    return false;
  }
  
  len_ = fread(buf_, 1, MAX_INI_SIZE-2, f);
  if(len_ >= MAX_INI_SIZE-2) r3dError("ini file %s is too big\n", lpFileName);
  buf_[len_] = 0;
  fclose(f);

  goodIniFile_ = true;
  
  Preprocess();
  return true;
}

void r3dIniFileReader::InvalidateFileName ()
{
	curIni_[0] = 0;
	goodIniFile_ = false;
}


void r3dIniFileReader::Preprocess()
{
  assert(buf_);
  
  for(int i=0; i<len_; i++) {
    if(buf_[i] == 0x0D) buf_[i] = 0;
    if(buf_[i] == 0x0A) buf_[i] = 0;
  }
  
  return;
}

const char* r3dIniFileReader::GetNextLine(const char* line)
{
  int i = line - buf_;

  // find first end of string
  for(; i<len_; i++) {
    if(buf_[i] == 0) break;
  }
  if(i >= len_) return NULL;

  // skip all following zero strings
  for(i = i+1; i<len_; i++) {
    if(buf_[i] != 0) break;
  }
  if(i >= len_) return NULL;
  return &buf_[i];
}


bool r3dIniFileReader::GetPrivateProfileString(const char* lpAppName, const char* lpKeyName, 
			  const char* lpDefault, char* lpReturnedString, int nSize, 
	                  const char* lpFileName)
{
  Init();

  r3dscpy(lpReturnedString, lpDefault);
  
  if(!ValidateFileName(lpFileName))
    return false;
    
  // those 2 can't be null in this implementation - we must have section & key
  assert(lpAppName);
  assert(lpKeyName);
  
  int len = strlen(lpAppName);

  // search for section
  const char* section = NULL;
  for(const char* line = buf_; line; line = GetNextLine(line)) 
  {
    if(line[0] == ';') 
      continue;

    if(line[0] == '[') {
      if(_strnicmp(line+1, lpAppName, len) == NULL) {
        if(line[len+1] == ']') {
          section = line;
          break;
        }
      }
    }
  }
  
  if(!section) {
    return false;
  }

  // search for actual key
  len = strlen(lpKeyName);
  for(const char* line = GetNextLine(section); line; line = GetNextLine(line)) 
  {
    if(line[0] == ';') 
      continue;

    if(line[0] == '[') {
      // found new section, abort
      return false;
    }

    if(_strnicmp(line, lpKeyName, len) != NULL)
      continue;
  
    // skip whitelines
    const char* p = line + len;

	if( *p >= 'a' && *p <= 'z'
			||
		*p >= 'A' && *p <= 'Z'
			||
		*p >= '0' && *p <= '9'
			||
		*p == '_'
		)
	{
		// must be some other identifier which shares part of the name
		continue;
	}

    for(; *p == ' ' || *p == '\t'; ++p) ;
    
    if(*p != '=') {
      r3dArtBug("ini: malformed string '%s' in file %s\n", line, curIni_);
      return false;
    }

    // skip whitespaces after '='
    for(p = p+1; *p == ' ' || *p == '\t'; ++p) ;

    r3dscpy(lpReturnedString, p);
    return true;
  }

  return false;
}


bool r3dGetPrivateProfileString(const char* lpAppName, const char* lpKeyName, const char* lpDefault, 
  char* lpReturnedString, int nSize, const char* lpFileName)
{
  bool res = _r3d_iniReader.GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
  //r3dOutToLog("ini: %s:[%s]:%s = %s (%s)\n", lpFileName, lpAppName, lpKeyName, lpReturnedString, res ? "ok" : "miss");
  return res;
}

static const  int  _r3d_inibufsize = 1024;
static char        _r3d_inibuf[_r3d_inibufsize];

void r3dCloseCFG_Cur ()
{
	_r3d_iniReader.InvalidateFileName ();
}

const char* r3dReadCFG_S(const char* FileName, const char* group, const char* name, const char* defaultvalue)
{
  r3dGetPrivateProfileString (group, name, defaultvalue, _r3d_inibuf, _r3d_inibufsize, FileName);

  return _r3d_inibuf;
}


int r3dReadCFG_I(const char* FileName, const char* group, const char* name, int defval)
{
  char defaultvalue[64];
  sprintf(defaultvalue,"%d", defval );

  r3dGetPrivateProfileString (group, name, defaultvalue, _r3d_inibuf, _r3d_inibufsize, FileName);
  return atoi(_r3d_inibuf);
}

float r3dReadCFG_F(const char* FileName, const char* group, const char* name, float defval)
{
  char defaultvalue[64];
  sprintf(defaultvalue,"%f", defval );

  r3dGetPrivateProfileString (group, name, defaultvalue, _r3d_inibuf, _r3d_inibufsize, FileName);
  return float(atof(_r3d_inibuf));
}

r3dVector r3dReadCFG_V(const char* FileName, const char* group, const char* name, const r3dPoint3D& DefVal)
{
  char defaultvalue[512];
  sprintf(defaultvalue,"%f %f %f", DefVal.X, DefVal.Y, DefVal.Z);

  r3dGetPrivateProfileString(group, name, defaultvalue, _r3d_inibuf, _r3d_inibufsize, FileName);

  float X,Y,Z;
  sscanf(_r3d_inibuf,"%f %f %f", &X, &Y, &Z);
  return r3dVector(X,Y,Z);
}


bool r3dCFGHas(const char* FileName, const char* group, const char* name)
{
	return r3dGetPrivateProfileString (group, name, "", _r3d_inibuf, _r3d_inibufsize, FileName);
}

void r3dWriteCFG_V(const char* FileName, const char* group, const char* name, const r3dPoint3D& DefVal)
{
 char defaultvalue[128];

 sprintf(defaultvalue,"%.4f %.4f %.4f", DefVal.x, DefVal.y, DefVal.z );

 WritePrivateProfileString(group, name, defaultvalue, FileName);
}


void r3dWriteCFG_I(const char* FileName, const char* group, const char* name, int val)
{
 char defaultvalue[64];

 sprintf(defaultvalue,"%d", val );

 WritePrivateProfileString (group, name, defaultvalue, FileName);
}


void r3dWriteCFG_F(const char* FileName, const char* group, const char* name, float val)
{
 char defaultvalue[64];

 sprintf(defaultvalue,"%.4f", val );

 WritePrivateProfileString (group, name, defaultvalue, FileName);
}

void r3dWriteCFG_S(const char* FileName, const char* group, const char* name, const char* Str)
{
 WritePrivateProfileString(group, name, Str, FileName);
}
