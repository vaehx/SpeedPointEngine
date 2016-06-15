#pragma once

#include <SPrerequisites.h>
#include "IRenderer.h"
#include "Vector3.h"

SP_NMSPACE_BEG

struct S_API ISkyBox
{
	virtual ~ISkyBox() {}

	virtual SResult InitGeometry(IRenderer* pRenderer, const SShaderResources& shaderResources = SShaderResources()) = 0;
	virtual void SetTexture(ITexture* pTexture) = 0;
	virtual void Clear() = 0;

	virtual void Update() = 0;
	virtual SRenderDesc* GetRenderDesc() = 0;

	// SkyBox always centered around camera
	virtual void SetPosition(const Vec3f& pos) = 0;
};

SP_NMSPACE_END
