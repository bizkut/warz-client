#pragma once

// localization manager, stores all strings used in game
enum Languages
{
	LANG_EN=0,
	LANG_RU,
	LANG_FR,
	LANG_DE,
	LANG_IT,
	LANG_SP
};
class LanguageManager
{
public:
	LanguageManager();
	~LanguageManager();

	void Init(Languages lang);
	void Destroy();

	const char* getString(const char* id, bool return_null_on_fail=false);
	
	bool isInit() const { return m_Inited; }

	Languages getCurrentLang() const { return m_lang; }

private:
	void readLanguageFile(const char* fileName);

	typedef r3dgameUnorderedMap(uint32_t, char*) StringHash;
	StringHash m_Dictionary;
	bool	m_Inited;
	Languages m_lang;
};
extern LanguageManager gLangMngr;