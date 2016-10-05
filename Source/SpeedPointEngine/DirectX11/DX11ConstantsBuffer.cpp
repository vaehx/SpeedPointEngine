#include "DX11ConstantsBuffer.h"
#include "DirectX11Renderer.h"

SP_NMSPACE_BEG

S_API SResult DX11ConstantsBuffer::Initialize(IRenderer* pRenderer, unsigned int sz)
{
	Clear();

	if (!IS_VALID_PTR(pRenderer) || pRenderer->GetType() != S_DIRECTX11)
		return S_INVALIDPARAM;

	m_pDXRenderer = dynamic_cast<DirectX11Renderer*>(pRenderer);
	
	m_pDXRenderer->D3D11_CreateConstantsBuffer(&m_pBuffer, sz);
	if (!IS_VALID_PTR(m_pBuffer))
		return S_ERROR;

	m_pData = new char[sz];
	m_Size = sz;

	return S_SUCCESS;
}

S_API void DX11ConstantsBuffer::Clear()
{
	SP_SAFE_RELEASE(m_pBuffer);
	m_pDXRenderer = 0;

	if (IS_VALID_PTR(m_pData))
		delete[] m_pData;

	m_pData = 0;
}

S_API void DX11ConstantsBuffer::Update()
{
	void* pData;
	m_pDXRenderer->D3D11_LockConstantsBuffer(m_pBuffer, &pData);
	memcpy(pData, m_pData, m_Size);
	m_pDXRenderer->D3D11_UnlockConstantsBuffer(m_pBuffer);
}

S_API void* DX11ConstantsBuffer::GetData() const
{
	return m_pData;
}

SP_NMSPACE_END