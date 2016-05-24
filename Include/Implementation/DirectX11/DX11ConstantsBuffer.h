#pragma once

#include <SPrerequisites.h>
#include <Abstract\IConstantsBuffer.h>

SP_NMSPACE_BEG

template<typename T>
class S_API DX11ConstantsBuffer : public IConstantsBuffer<T>
{
private:
	ID3D11Buffer* m_pBuffer;
	DirectX11Renderer* m_pDXRenderer;

public:
	DX11ConstantsBuffer()
		: m_pBuffer(0),
		m_pDXRenderer(0)
	{
	}

	virtual ~DX11ConstantsBuffer()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void Clear();

	virtual void Update();

	ID3D11Buffer* GetBuffer() const
	{
		return m_pBuffer;
	}
};

SP_NMSPACE_END