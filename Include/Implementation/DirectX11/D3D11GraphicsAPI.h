//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R.
//	(c) 2011-2015, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////


#include <Abstract\IGraphicsAPI.h>
#include <Abstract\CLog.h>
#include <d3d11_2.h>


/////
// WARN: THIS D3D11 IMPLEMENTATION WAS WRITTEN USING ID3D11Device2, WHICH IS ONLY SUPPORT ON WIN8.1+ !
/////

SP_NMSPACE_BEG

class S_API D3D11GraphicsAPI : public IGraphicsAPI
{
private:
	ID3D11Device2* m_pD3D11Device2;
	ID3D11DeviceContext2* m_pD3D11DeviceContext2;

public:
	virtual void SetConstantBuffers(EShader shader, unsigned int startSlot, unsigned int numCBs, IGAPIConstantsBuffer* pCBs)
	{
		//m_pD3D11DeviceContext2->PSSetConstantBuffers1(/* OFFSETS ???*/);
	}

	virtual EGAPIType GetType() const
	{
		return eGAPI_D3D11;
	}

public:
	ID3D11Device2* GetDevice()
	{
		return m_pD3D11Device2;
	}

	ID3D11DeviceContext2* GetDeviceContext()
	{
		return m_pD3D11DeviceContext2;
	}
};




class S_API D3D11ConstantsBuffer : public IGAPIConstantsBuffer
{
private:
	D3D11GraphicsAPI* m_pAPI;
	ID3D11Buffer* m_pBuffer;

public:
	D3D11ConstantsBuffer()
		: m_pAPI(0),
		m_pBuffer(0)
	{
	}

	virtual SResult Init(IGraphicsAPI* pAPI, unsigned int typeByteSize)
	{
		if (!CheckGraphicsAPI(pAPI, eGAPI_D3D11) || typeByteSize == 0)
			return S_INVALIDPARAM;

		m_pAPI = (D3D11GraphicsAPI*)pAPI;

		D3D11_BUFFER_DESC cbDesc;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.ByteWidth = typeByteSize;

		if (Failure(m_pAPI->GetDevice()->CreateBuffer(&cbDesc, 0, &m_pBuffer)))
			return CLog::Log(S_ERROR, "Failed Create constants buffer! See stdout for more info.");

		return S_SUCCESS;
	}


	virtual void Clear() = 0;

	virtual void SetData(void* pData) = 0;
	virtual void* GetData() = 0;
};



SP_NMSPACE_END