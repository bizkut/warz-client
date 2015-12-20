#ifndef __PWAR_OBJECT_TREE_H
#define __PWAR_OBJECT_TREE_H

class obj_Tree : public GameObject
{
	DECLARE_CLASS(obj_Tree, GameObject)

  public:
	  //r3dMesh			*Mesh;
	  //r3dTexture		*Tex;
	  //r3dTexture		*BillboardTex;
	  virtual	BOOL		Load(const char *name);
	  void					AppendMaterials(r3dgfxVector(r3dMaterial*)& materials);

  public:
	  obj_Tree();
	  virtual ~obj_Tree();

	  void DoDraw();
	  void DoDepthDraw();
	  void DoDrawShadows();

virtual	BOOL		OnCreate();

virtual	BOOL		Update();

//virtual	void 		SetPosition(const r3dPoint3D& pos);

virtual	BOOL		OnEvent(int event, void *data);

virtual void		AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) OVERRIDE;
virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

virtual	BOOL		CheckCastRay(const r3dPoint3D& vStart, const r3dPoint3D& vRay, float fRayLen, float *fDist, int *facen);

};

#endif	// __PWAR_OBJECT_TREE_H
