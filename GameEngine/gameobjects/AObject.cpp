#include "r3dPCH.h"
#include "r3d.h"


#include "AObject.h"
#include "GameObj.h"

IMPLEMENT_CLASS(AObject, "AObject", "Core")

class AObjectTable
{
  private:
static	AObjectTable	*pInstance;
	AObjectTable();
	~AObjectTable();

  public:
static	AObjectTable*	This();

	struct entry_s
	{
	  int		ID;		// same index as idx in TableEntries[idx]
	  AClass	*Class;
	};
	#define	MAX_REGISTERED_UOBJECTS	1024
	entry_s		table[MAX_REGISTERED_UOBJECTS];
	int		entries;

  public:
	int		RegisterClass(AClass *cl);

	int		GetClassID(const char* name, const char* type);
	AObject*	CreateObject(int ID);
	stringlist_t GetRegisteredClassesList () const;
};

	AObjectTable*  	AObjectTable::pInstance = NULL;

AObjectTable::AObjectTable()
{
  //r3dOutToLog("AObjectTable Inited\n");
  
  entries = 0;
}

AObjectTable::~AObjectTable()
{
  //FIXME: delete pInstance at the program termination.. create list of classes to destroy, or something.
  entries = 0;
}

AObjectTable* AObjectTable::This()
{
  if(!pInstance)
    pInstance = game_new AObjectTable;
  return pInstance;
}


int AObjectTable::RegisterClass(AClass *cl)
{
  if(entries + 1 >= MAX_REGISTERED_UOBJECTS) 
  {
    r3dArtBug("ERROR: AObjectTable - MAX_REGISTERED_UOBJECTS reached\n\n");
    return 0;
  }
   
  table[entries].ID    = entries;
  table[entries].Class = cl;
  cl->ID               = entries;

  //r3dOutToLog("AObjectTable: class \"%s\" registered\n", cl->Name.c_str());

  entries++;
  return 1;
}

AObject *AObjectTable::CreateObject(int ID)
{
	AObject	 	*obj;

	if(ID < 0 || ID >= entries || table[ID].ID != ID) {
		r3dError("GetClassID: class %d isn't present\n", ID);
	}

	obj        = (AObject *)table[ID].Class->ClassConstructor();
	obj->Class = table[ID].Class;

	return obj;
}

int AObjectTable::GetClassID(const char* name, const char* type)
{
	for(int i=0; i<entries; i++) {
		if(table[i].Class->Name == name && table[i].Class->Type == type)
			return i;
	}

	return -1;
}

stringlist_t AObjectTable::GetRegisteredClassesList () const
{
	stringlist_t list;
	for(int i=0; i<entries; i++) 
		list.push_back(table[i].Class->Name.c_str());

	return list;
}

int		AObjectTable_RegisterClass(AClass *cl)		{ return AObjectTable::This()->RegisterClass(cl); }
int		AObjectTable_GetClassID(const char* name, const char* type)	{ return AObjectTable::This()->GetClassID(name, type); }
AObject*	AObjectTable_CreateObject(int id)		{ return AObjectTable::This()->CreateObject(id); }
extern stringlist_t AObjectTable_GetRegisteredClassesList () {return AObjectTable::This()->GetRegisteredClassesList();}
