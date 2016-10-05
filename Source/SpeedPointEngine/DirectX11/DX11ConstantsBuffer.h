//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Abstract\IConstantsBuffer.h>
#include <Abstract\SPrerequisites.h>

struct ID3D11Buffer;

SP_NMSPACE_BEG

class S_API DX11Renderer;

class S_API DX11ConstantsBuffer : public IConstantsBuffer
{
private:
	ID3D11Buffer* m_pBuffer;
	DX11Renderer* m_pDXRenderer;
	void* m_pData;
	unsigned int m_Size; // num bytes in m_pData

public:
	DX11ConstantsBuffer()
		: m_pBuffer(0),
		m_pDXRenderer(0),
		m_pData(0)
	{
	}

	virtual ~DX11ConstantsBuffer()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer, unsigned int sz);
	virtual void Clear();

	virtual void Update();

	virtual void* GetData() const;

	ID3D11Buffer* GetBuffer() const
	{
		return m_pBuffer;
	}
};

SP_NMSPACE_END
