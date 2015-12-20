#pragma once

class CVar
{
  public:
	char		name[128];
	char		data[1024];
  public:
	CVar()
	{
	  *name = 0;
	  *data = 0;
	};
};

class CConfig
{
  private:
	char		fname[MAX_PATH];
	CVar		vars[128];
	int		n_vars;
	
	CVar*		GetVar(const char* name);
	CVar*		AddVar(const char* name, const char* val);

  public:
	CConfig();
	~CConfig();
	
	void		LoadConfig(const char* fname);
	void		SaveConfig();
	
	const char*	GetStr(const char* name, const char* def);
	void		SetStr(const char* name, const char* val);

	const int	GetInt(const char* name, int def);
	void		SetInt(const char* name, int val);
};

extern	CConfig		g_Config;