#pragma once

#include <Abstract\ISkyBox.h>
#include <Abstract\I3DEngine.h>

SP_NMSPACE_BEG

struct S_API IRenderer;

// SpeedPoint SkyBox Implementation
class S_API CSkyBox : public ISkyBox
{
private:
	SRenderDesc m_RenderDesc;
	IRenderer* m_pRenderer;
	Vec3f m_Position;

	void InitRenderDesc();

public:
	CSkyBox();
	~CSkyBox()
	{
		Clear();
	}

	virtual SResult InitGeometry(IRenderer* pRenderer, const SShaderResources& shaderResources = SShaderResources());
	virtual void SetTexture(ITexture* pTexture);
	virtual void Clear();

	virtual void SetPosition(const Vec3f& pos);

	// Should be called by the renderer
	virtual void Update();

	virtual SRenderDesc* GetRenderDesc();
};

SP_NMSPACE_END