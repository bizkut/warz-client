#ifndef __BULLET_SHELL_MANAGER_H__
#define __BULLET_SHELL_MANAGER_H__
// class for managing and rendering bullet shells

enum BulletShellType
{
	BST_Pistol=0,
	BST_Rifle,
	BST_Shotgun,

	BST_NumElements
};

class BulletShell : public PhysicsCallbackObject
{
public:
	BulletShell();
	~BulletShell();
	
	void Init(const r3dPoint3D& pos, const r3dPoint3D& vel, const PhysicsObjectConfig& config, r3dMesh* mesh, BulletShellType type, const D3DXMATRIX& rotation);
	void Destroy();

	void Update();

	bool Active() { return m_physObj!=0; }

	const r3dPoint3D& getPosition() { return m_Pos; }
	const D3DXMATRIX& getDrawMatrix() { return m_RenderMatrix; }

	virtual	void OnCollide(PhysicsCallbackObject *obj, CollisionInfo &trace);
	virtual GameObject* isGameObject() { return NULL; }
	virtual r3dMesh* hasMesh() { return NULL; }

	BulletShellType m_Type;
private:
	BasePhysicsObject* m_physObj; 
	r3dPoint3D m_Pos;
	D3DXMATRIX m_RenderMatrix;
	float m_StartTime;
	bool	m_PlayedBrassSound;
};

class BulletShellMngr
{
public:
	BulletShellMngr();
	~BulletShellMngr();

	void Update();
	void AppendRenderables( RenderArray(&render_arrays)[rsCount], const r3dCamera& Cam );

	void AddShell(const r3dPoint3D& pos, const r3dPoint3D& vel, const D3DXMATRIX& rotation, BulletShellType shellType);
private:
	static const int MAX_SHELLS = 512;
	BulletShell m_Shells[MAX_SHELLS];
	int			m_numActiveShells;

	r3dMesh*	m_shellMeshes[BST_NumElements]; 
	PhysicsObjectConfig m_shellPhysConfigs[BST_NumElements];
};


#endif //__BULLET_SHELL_MANAGER_H__