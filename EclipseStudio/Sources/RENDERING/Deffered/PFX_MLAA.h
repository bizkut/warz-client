#pragma once

void InitMLAA();
void CloseMLAA();
void UpdateMLAA();

extern r3dScreenBuffer* gBuffer_MLAA_LinesH;
extern r3dScreenBuffer* gBuffer_MLAA_LinesV;

extern float MLAA_DiscontFactor;

class PFX_MLAA_DiscontMap : public PostFX
{
public:
	PFX_MLAA_DiscontMap();
	~PFX_MLAA_DiscontMap();
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl()							OVERRIDE;
};

class PFX_MLAA_LineDetect : public PostFX
{
public:
	PFX_MLAA_LineDetect();
	~PFX_MLAA_LineDetect();

	void	ResetLevel	();
protected:
	float	GetLevel	();

	virtual void InitImpl()								OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;
private:
	float	m_fLevel;
};

class PFX_MLAA_LineDetectH : public PFX_MLAA_LineDetect
{
public:
	PFX_MLAA_LineDetectH();
	~PFX_MLAA_LineDetectH();
protected:
	virtual void InitImpl()								OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;
};

class PFX_MLAA_LineDetectV : public PFX_MLAA_LineDetect
{
public:
	PFX_MLAA_LineDetectV();
	~PFX_MLAA_LineDetectV();
protected:
	virtual void InitImpl()								OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;
};

class PFX_MLAA_AlphaCalc : public PostFX
{
public:
	PFX_MLAA_AlphaCalc();
	~PFX_MLAA_AlphaCalc();
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl()							OVERRIDE;
};

class PFX_MLAA_Blend : public PostFX
{
public:
	PFX_MLAA_Blend();
	~PFX_MLAA_Blend();
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl()							OVERRIDE;
};