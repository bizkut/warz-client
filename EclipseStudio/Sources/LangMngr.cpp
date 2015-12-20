#include "r3dPCH.h"
#include "r3d.h"

#include "LangMngr.h"

LanguageManager gLangMngr;

LanguageManager::LanguageManager()
{
	m_Inited = false;
	m_lang = LANG_EN;
}

LanguageManager::~LanguageManager()
{

}

void LanguageManager::Init(Languages lang)
{
	r3d_assert(!m_Inited);
	m_Inited = true;
	
	//lang = LANG_RU;

	if(lang == LANG_EN)
		readLanguageFile("data/LangPack/english.lang");
	else if(lang == LANG_RU)
		readLanguageFile("data/LangPack/russian.lang");
	else if(lang == LANG_FR)
		readLanguageFile("data/LangPack/french.lang");
	else if(lang == LANG_DE)
		readLanguageFile("data/LangPack/german.lang");
	else if(lang == LANG_IT)
		readLanguageFile("data/LangPack/italian.lang");
	else if(lang == LANG_SP)
		readLanguageFile("data/LangPack/spanish.lang");
	else
		r3dError("Unknown language %d\n", lang);

	m_lang = lang;
}

void LanguageManager::Destroy()
{
	r3d_assert(m_Inited);
	m_Inited = false;

	for(StringHash::iterator it=m_Dictionary.begin(); it!=m_Dictionary.end(); ++it)
	{
		SAFE_DELETE_ARRAY((*it).second);
	}
	m_Dictionary.clear();
}

void LanguageManager::readLanguageFile(const char* fileName)
{
	r3dFile* f = r3d_open(fileName, "rb");
	if(!f)
		r3dError("Cannot open language file '%s'", fileName);

	char buf[4096];
	char tempStringID[4097];
	while(fgets(buf, 4096, f))
	{
		// skip comments
		if(buf[0]=='/' && buf[1] =='/')
			continue;
		if((unsigned char)buf[0]==0xEF && (unsigned char)buf[1]==0xBB && (unsigned char)buf[2]==0xBF && buf[3]=='/' && buf[4]=='/')
			continue;
		// skip sections
		if(buf[0]=='[')
			continue;
		if(strlen(buf) < 5)
			continue;
		// remove trailing \n
		if(buf[strlen(buf)-1] == '\n') 
			buf[strlen(buf)-1]=0;
		// remove trailing \r
		if(buf[strlen(buf)-1] == '\r') 
			buf[strlen(buf)-1]=0;

		char* equalSign = strstr(buf, "=");
		if(equalSign == NULL)
			r3dError("Localization: bad string format '%s' - missing '='", buf);

		// id part should always be in english, so that one symbol = 1 char
		int idLen = equalSign-buf;
		r3dscpy_s(tempStringID, idLen+1, buf);
		tempStringID[idLen] = 0;

		// convert translation string into wide char
		wchar_t* tempStr = utf8ToWide(buf);
		// find any \n in string and replace them with '\n' symbol
		wchar_t* endLineSymbol = NULL;
		while(endLineSymbol=wcsstr(tempStr, L"\\n"))
		{
			*endLineSymbol='\n';
			int len = wcslen(tempStr)-((endLineSymbol)-tempStr)-4;
			if(len>2)
			{
				for(wchar_t* s = endLineSymbol+1; ; ++s)
				{
					*s = *(s+1);
					if(*s==0)
						break;
				}
			}
			else
				*(endLineSymbol+1) = '\0';
		}

		char* tempStr2 = wideToUtf8(tempStr);
		int translLen = strlen(tempStr2)-(idLen+1);
		char* translatedString = game_new char[translLen+1];
		r3dscpy_s(translatedString, (translLen+1)*sizeof(char), &tempStr2[idLen+1]);
		translatedString[translLen] = 0;

		std::pair<StringHash::iterator, bool> result = m_Dictionary.insert(std::pair<uint32_t, char*>(r3dHash::MakeHash(tempStringID), translatedString));
#ifndef FINAL_BUILD
		// check that we don't have two identical string ID
		if(result.second==false)
			r3dError("Localization: duplicate string ID='%s'", tempStringID);
#endif
	}

	fclose(f);
}

const char* LanguageManager::getString(const char* id, bool return_null_on_fail)
{
	StringHash::iterator it = m_Dictionary.find(r3dHash::MakeHash(id));
	if(it==m_Dictionary.end())
	{
		if(return_null_on_fail)
			return NULL;

#ifndef FINAL_BUILD
		r3dOutToLog("!!!!!!!!! Localization: Failed to find string id='%s'\n", id);
#endif

		// add new entry with translated string same as id
		char* translatedString = game_new char[strlen(id)+1];
		r3dscpy(translatedString, id);
		std::pair<StringHash::iterator, bool> result = m_Dictionary.insert(std::pair<uint32_t, char*>(r3dHash::MakeHash(id), translatedString));
		r3d_assert(result.second); // should always succeed!
		
		return translatedString;
	}
	return (*it).second;
}
