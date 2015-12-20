//=========================================================================
//	Module: ApexActor.h
//	Created by Roman E. Marchenko <roman.marchenko@gmail.com>
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#if APEX_ENABLED
//////////////////////////////////////////////////////////////////////////

namespace physx
{
	namespace apex
	{
		class NxDestructibleActor;
		class NxApexAssetPreview;
		class NxApexRenderable;
	}
	
	namespace pubfnd2
	{
		class PxMat44;
	}
}

/** Apex actor types. */
enum ApexActorTypes
{
	TypeApexDestructible = 0,
	TypeApexPreview
};

/**	Actor creation parameters structure. */
struct ApexActorParameters
{
	r3dString assetPath;
	D3DXMATRIX pose;
	void *userData;

	ApexActorParameters()
	: userData(0)
	{}

	void SetPose(const r3dPoint3D &pos, const r3dPoint3D &rotVec);
};

//-------------------------------------------------------------------------
//	Apex actor base class
//-------------------------------------------------------------------------

/**	Maximum distinct material allowed for single apex actor. */
#define APEX_ACTOR_MAX_MATERIALS 4

class ApexActorBase
{
protected:
	const ApexActorParameters params;
	
	r3dMaterial * materials[APEX_ACTOR_MAX_MATERIALS];
	uint32_t numMaterials;

	virtual physx::apex::NxApexRenderable * GetRenderable() const = 0;
	/**	Next update should rewrite buffers (device lost handling). */
	bool rewriteBuffers;

public:
	explicit ApexActorBase(const ApexActorParameters &ap);
	virtual ~ApexActorBase() = 0;
	virtual void Update();
	virtual void Draw();
	virtual void DrawShadow();
	virtual void SetPosition(const r3dVector &pos);
	virtual void SetRotation(const r3dVector &angles);
	virtual r3dBoundBox GetBBox() const;
	/**	If object is Apex actor than create apex preview, and vice versa. */
	virtual ApexActorBase * ConvertToOpposite() = 0;
	virtual ApexActorTypes Type() const = 0;
	virtual ApexActorBase * Clone() const = 0;

	/** see apex sdk for applyRadiusDamage function */
	virtual void ApplyAreaDamage(float damage, float momentum, const r3dPoint3D& position, float radius, bool falloff) {}
	virtual void ApplyDamage(float damage, float momentum, const r3dPoint3D &pos, const r3dPoint3D &direction) {}

	/** Add material to the internal array. If given material already exists, do nothing.*/
	void AddMaterial(r3dMaterial *m);
	r3dMaterial * GetMaterial(uint32_t faceID);

	const r3dString & GetAssetPath() const { return params.assetPath; }
	const ApexActorParameters GetCreationParameters() const { return params; }
	void RewriteBuffers() { rewriteBuffers = true; }
};

//////////////////////////////////////////////////////////////////////////

/**	Create apex actor. */
ApexActorBase * CreateApexActor(const ApexActorParameters &params, bool makePreview);

//////////////////////////////////////////////////////////////////////////

namespace Private
{
	//-------------------------------------------------------------------------
	//	Apex destructible actor
	//-------------------------------------------------------------------------
	class ApexActor: public ApexActorBase
	{
		mutable physx::apex::NxDestructibleActor *actor;
		physx::apex::NxDestructibleActor * CreateActor();

	protected:
		physx::apex::NxApexRenderable * GetRenderable() const;

	public:
		explicit ApexActor(const ApexActorParameters &ap);
		~ApexActor();
		ApexActorBase * ConvertToOpposite();
		virtual ApexActorTypes Type() const { return TypeApexDestructible; }
		ApexActorBase * Clone() const;
		void ApplyAreaDamage(float damage, float momentum, const r3dPoint3D& position, float radius, bool falloff);
		void ApplyDamage(float damage, float momentum, const r3dPoint3D &pos, const r3dPoint3D &direction);
	};

	//-------------------------------------------------------------------------
	//	Apex preview mesh. Used for editor placements, because there is no
	//	ability to move actual apex actor
	//-------------------------------------------------------------------------
	class ApexPreviewActor: public ApexActorBase
	{
		mutable physx::apex::NxApexAssetPreview *prv;
		physx::apex::NxApexAssetPreview * CreatePreviewActor();

	protected:
		physx::apex::NxApexRenderable * GetRenderable() const;

	public:
		explicit ApexPreviewActor(const ApexActorParameters &ap);
		~ApexPreviewActor();
		void SetPosition(const r3dVector &pos);
		void SetRotation(const r3dVector &angles);
		ApexActorBase * ConvertToOpposite();
		virtual ApexActorTypes Type() const { return TypeApexPreview; }
		ApexActorBase * Clone() const;
	};
}

#endif