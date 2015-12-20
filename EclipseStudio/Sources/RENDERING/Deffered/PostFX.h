#pragma once

struct PostFXData
{
	enum
	{
		MAX_VS_CONSTANTS	= 32,
		MAX_EXTRA_TEXTURES	= 8
	};

	enum EVSType
	{
		VST_DEFAULT,
		VST_RESTORE_W,
		VST_CUSTOM
	};

	static const int WRONG_SHADER_ID = -1;	

	int			PixelShaderID;
	int			VertexShaderID;

	EVSType		VSType;
	float		TexTransform[ 4 ];

	PostFXData();
};

#pragma warning( disable: 4355 ) // 'this' : used in base member initializer list

struct PostFX
{
public:
	typedef PostFX Parent;

public:
	template < typename T >
	explicit PostFX( T* child );

	virtual ~PostFX();

public:
	void				Init();
	void				Close();
	const PostFXData&	Prepare(	r3dScreenBuffer* dest, 
									r3dScreenBuffer* src );
	void				Finish();
	void				PushDefaultSettings();
	const char*			GetName() const ;

private:
	virtual void		InitImpl() = 0;
	virtual	void		CloseImpl() = 0;
	virtual void		PrepareImpl(	r3dScreenBuffer* dest, 
										r3dScreenBuffer* src ) = 0;

	virtual void		FinishImpl() = 0;

	virtual void		PushDefaultSettingsImpl();

protected:
	PostFXData			mData;
	const char*			mName;
#ifdef R3DPROFILE_ENABLED
	uint32_t			nameHash;
#endif

	int					mSettingsPushed;
};

//------------------------------------------------------------------------

template < typename T >
/*explicit*/
PostFX::PostFX( T* /*child*/ )
: mName( typeid(T).name() )
, mSettingsPushed( 0 )
#ifdef R3DPROFILE_ENABLED
, nameHash(0)
#endif
{

}

//------------------------------------------------------------------------

R3D_FORCEINLINE
const char* PostFX::GetName() const
{
	return mName ;
}