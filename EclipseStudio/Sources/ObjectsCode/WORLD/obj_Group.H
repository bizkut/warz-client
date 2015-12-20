#ifndef __GO_Group_H
#define __GO_Group_H

class obj_Group
{
public:
	r3dTL::TArray < GameObject * > m_dObjects;
	r3dTL::TArray < GameObject * > m_dPreviewObjects;
	int								m_iPreviewObjectsInGroup;

	char							m_szName[256];

	static r3dTL::TArray < obj_Group * > m_dGroups;

	int							m_iIndexInFile;

private:

	r3dTL::TArray < uint32_t > m_dFixupObjInfo;
	bool						m_bGroupJustCreated;
	bool						m_bPreviewMode;

public:

	void				SetPreviewMode ( bool bPreview );
	bool				GetPreviewMode () { return m_bPreviewMode; }
	void				SetPreviewObjectsPos ( r3dVector vPos, bool dropOnGround );
	bool				IsGroupJustCreated () const { return m_bGroupJustCreated; }
	void				ResetJustCreatedFlag ( bool bResetPicked = true );
	void				SetJustCreatedFlag () {m_bGroupJustCreated = true;}
	bool				IsObjectInGroup ( GameObject * pObj ) const;
	void				AddObject ( GameObject * pObj );
	void				RemoveObject ( GameObject * pObj );

	static void			LoadFromFile ( const char * sFile );	
	static void			SaveToFile ( const char * sFile );	
	static void			Save();

	static obj_Group *  CreateNewGroup ();
	static void			DeleteGroup ( obj_Group & group );

	static void			Unsubscribe ( GameObject * pObj );

public:
						obj_Group ();
						~obj_Group ();
	
	void				LoadGroupObjects ();
	void				UnloadGroupObjects ();

	void				ReadSerializedData(pugi::xml_node& node, bool bObjectsOnlyLoad = false );
	void				WriteSerializedData(pugi::xml_node& node);
};


#endif	// __GO_Group_H

