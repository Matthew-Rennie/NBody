#pragma once
#include "d3d11.h"

#define CONSTANT_BUFFER_ALIGNMENT 16

template<size_t size>
class ConstantBuffer
{
public:

	ConstantBuffer();
	ConstantBuffer(ConstantBuffer&) = delete;
	ConstantBuffer& operator=(ConstantBuffer&) = delete;

	~ConstantBuffer();

	void initBuffer(ID3D11Device* device);
	void releaseBuffer();

	//ID3D11Buffer* buffer() noexcept { return m_buffer; }
	void* mappedResource() { return m_mapped_resource.pData; }

	void setBuffer_CS(ID3D11DeviceContext* deviceContext);
	void setBuffer_DS(ID3D11DeviceContext* deviceContext);
	void setBuffer_GS(ID3D11DeviceContext* deviceContext);
	void setBuffer_HS(ID3D11DeviceContext* deviceContext);
	void setBuffer_PS(ID3D11DeviceContext* deviceContext);
	void setBuffer_VS(ID3D11DeviceContext* deviceContext);

	UINT& startSlot() noexcept { return m_startSlot; }
	UINT& numBuffers() noexcept { return m_numBuffers; }

	HRESULT map(ID3D11DeviceContext* deviceContext);
	void unmap(ID3D11DeviceContext* deviceContext);

protected:

	ID3D11Buffer* m_buffer = nullptr;
	D3D11_MAPPED_SUBRESOURCE m_mapped_resource = { NULL,NULL,NULL };
	UINT m_startSlot = 0;
	UINT m_numBuffers = 1;
};

template<size_t size>
ConstantBuffer<size>::ConstantBuffer()
{
	static_assert(size % CONSTANT_BUFFER_ALIGNMENT == 0, "Constant buffer must be 16 byte aligned");
}

template<size_t size>
ConstantBuffer<size>::~ConstantBuffer()
{
	releaseBuffer();
}

template<size_t size>
void ConstantBuffer<size>::setBuffer_HS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->HSSetConstantBuffers(m_startSlot, m_numBuffers, &m_buffer);
}

template<size_t size>
void ConstantBuffer<size>::setBuffer_GS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->GSSetConstantBuffers(m_startSlot, m_numBuffers, &m_buffer);
}

template<size_t size>
void ConstantBuffer<size>::setBuffer_DS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->DSSetConstantBuffers(m_startSlot, m_numBuffers, &m_buffer);
}

template<size_t size>
void ConstantBuffer<size>::setBuffer_CS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->CSSetConstantBuffers(m_startSlot, m_numBuffers, &m_buffer);
}

template<size_t size>
void ConstantBuffer<size>::setBuffer_PS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetConstantBuffers(m_startSlot, m_numBuffers, &m_buffer);
}

template<size_t size>
void ConstantBuffer<size>::setBuffer_VS(ID3D11DeviceContext* deviceContext)
{
	deviceContext->VSSetConstantBuffers(m_startSlot, m_numBuffers, &m_buffer);
}

template<size_t size>
HRESULT ConstantBuffer<size>::map(ID3D11DeviceContext* deviceContext)
{
	return deviceContext->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mapped_resource);
}

template<size_t size>
void ConstantBuffer<size>::unmap(ID3D11DeviceContext* deviceContext)
{
	deviceContext->Unmap(m_buffer, 0);
}

template<size_t size>
void ConstantBuffer<size>::initBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = NULL;
	bufferDesc.StructureByteStride = NULL;
	device->CreateBuffer(&bufferDesc, NULL, &m_buffer);
}

template<size_t size>
void ConstantBuffer<size>::releaseBuffer()
{
	if (m_buffer)
	{
		m_buffer->Release();
		m_buffer = NULL;
	}
}
