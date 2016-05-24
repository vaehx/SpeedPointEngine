#include <Implementation\DirectX11\DX11ConstantsBuffer.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>

SP_NMSPACE_BEG

template<typename T>
S_API SResult DX11ConstantsBuffer<T>::Initialize(IRenderer* pRenderer)
{
	if (!IS_VALID_PTR(pRenderer) || pRenderer->GetType() != S_DIRECTX11)
		return S_INVALIDPARAM;

	m_pDXRenderer = dynamic_cast<DirectX11Renderer*>(pRenderer);
	
	m_pDXRenderer->D3D11_CreateConstantsBuffer(&m_pBuffer, sizeof(T));
	if (!IS_VALID_PTR(m_pBuffer))
		return S_ERROR;
}

template<typename T>
S_API void DX11ConstantsBuffer<T>::Clear()
{
	SP_SAFE_RELEASE(m_pBuffer);
	m_pDXRenderer = 0;
}

template<typename T>
S_API void DX11ConstantsBuffer<T>::Update()
{
	void* pData;
	m_pDXRenderer->D3D11_LockConstantsBuffer(m_pBuffer, &pData);
	memcpy(pData, (void*)&m_Constants, sizeof(m_Constants));
	m_pDXRenderer->D3D11_UnlockConstantsBuffer(m_pBuffer);
}

SP_NMSPACE_END