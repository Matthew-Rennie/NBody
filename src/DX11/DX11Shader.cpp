#include "stdafx.hpp"
#include "DX11Shader.h"
#include <string>
#include <D3Dcompiler.h>
#include "core/BaseRenderer.h"
#include "DX11_Renderer.h"
#include <stdexcept>

dx11::DX11Shader::DX11Shader(core::BaseRenderer* renderer)
{
	DX11_Renderer* dx11_renderer = dynamic_cast<DX11_Renderer*>(renderer);

	if (!dx11_renderer)
		throw std::runtime_error("invalid rendere type");

	m_device = dx11_renderer->get_d3d()->getDevice();
}

dx11::DX11Shader::~DX11Shader()
{
	Release();
}

void dx11::DX11Shader::Release()
{
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}
}

bool dx11::DX11Shader::load_PS(wchar_t* filename)
{
	ID3DBlob* pixelShaderBuffer;

	// check file extension for correct loading function.
	std::wstring fn(filename);
	std::string::size_type idx;
	std::wstring extension;

	idx = fn.rfind('.');

	if (idx != std::string::npos)
	{
		extension = fn.substr(idx + 1);
	}
	else
	{
		// No extension found
		//MessageBox(hwnd, L"Error finding pixel shader file", L"ERROR", MB_OK);
		exit(0);
	}

	// Load the texture in.
	if (extension != L"cso")
	{
		// TODO @matthew -  put proper error handling here
		//MessageBox(hwnd, L"Incorrect pixel shader file type", L"ERROR", MB_OK);
		exit(0);
	}

	// Reads compiled shader into buffer (bytecode).
	HRESULT result = D3DReadFileToBlob(filename, &pixelShaderBuffer);
	if (result != S_OK)
	{
		MessageBox(nullptr, filename, L"File not found", MB_OK);
		exit(0);
	}
	// Create the pixel shader from the buffer.
	m_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	return true;
}

bool dx11::DX11Shader::load_VS(wchar_t* filename)
{
	ID3DBlob* vertexShaderBuffer;

	unsigned int numElements;

	vertexShaderBuffer = nullptr;

	// check file extension for correct loading function.
	std::wstring fn(filename);
	std::string::size_type idx;
	std::wstring extension;

	idx = fn.rfind('.');

	if (idx != std::string::npos)
	{
		extension = fn.substr(idx + 1);
	}
	else
	{
		// No extension found
		//MessageBox(hwnd, L"Error finding vertex shader file", L"ERROR", MB_OK);
		exit(0);
	}

	// Load the texture in.
	if (extension != L"cso")
	{
		// TODO @matthew -  put proper error handling here
		//MessageBox(hwnd, L"Incorrect vertex shader file type", L"ERROR", MB_OK);
		exit(0);
	}

	// Reads compiled shader into buffer (bytecode).
	HRESULT result = D3DReadFileToBlob(filename, &vertexShaderBuffer);
	if (result != S_OK)
	{
		MessageBox(nullptr, filename, L"File ERROR", MB_OK);
		exit(0);
	}

	// Create the vertex shader from the buffer.
	m_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the MeshClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	m_device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	return true;
}

void dx11::DX11Shader::run(ID3D11DeviceContext* context, UINT index_count)
{
	// Set the vertex input layout.
	context->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render.
	context->VSSetShader(m_vertexShader, nullptr, 0);
	context->PSSetShader(m_pixelShader, nullptr, 0);
	context->CSSetShader(nullptr, nullptr, 0);

	// if Hull and Domain shader is not null then set HS and DS
	if (m_hullShader && m_domainShader)
	{
		context->HSSetShader(m_hullShader, nullptr, 0);
		context->DSSetShader(m_domainShader, nullptr, 0);
	}
	else
	{
		context->HSSetShader(nullptr, nullptr, 0);
		context->DSSetShader(nullptr, nullptr, 0);
	}

	// if geometry shader is not null then set GS
	if (m_geometryShader)
	{
		context->GSSetShader(m_geometryShader, nullptr, 0);
	}
	else
	{
		context->GSSetShader(nullptr, nullptr, 0);
	}

	// Render the triangle.
	context->DrawIndexed(index_count, 0, 0);
}