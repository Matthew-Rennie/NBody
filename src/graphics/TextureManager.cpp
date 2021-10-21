#include "stdafx.hpp"
#include "TextureManager.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include <fstream>
#include "DX11/DX11Renderer.h"
#include "d3d11.h"

graphics::TextureManager::TextureManager(dx11::DX11Renderer* renderer)
{
	m_renderer = renderer;
}

bool graphics::TextureManager::AddTexture(const std::wstring& filename, const std::string& name)
{
	Texture texture;
	texture.filename = filename;

	// return if already loaded
	if (m_textures.find(name) != m_textures.end())
		return true;

	HRESULT result;

	// check if file exists
	if (texture.filename.empty() || !does_file_exist(texture.filename.c_str()))
	{
		// change default texture
		//filename = L"../res/DefaultDiffuse.png";
		MessageBox(nullptr, L"Texture filename does not exist", L"ERROR", MB_OK);
		return false;
	}

	// check file extension for correct loading function.
	std::string::size_type idx;
	std::wstring extension;

	idx = texture.filename.rfind('.');

	if (idx != std::string::npos)
	{
		extension = texture.filename.substr(idx + 1);
	}
	else
	{
		// No extension found
		return false;
	}
	// Load the texture in.
	if (extension == L"dds")
	{
		result = DirectX::CreateDDSTextureFromFile(m_renderer->get_d3d()->getDevice(), m_renderer->get_d3d()->getDeviceContext(), texture.filename.c_str(), nullptr, &texture.dx11_texture);
	}
	else
	{
		result = DirectX::CreateWICTextureFromFile(m_renderer->get_d3d()->getDevice(), m_renderer->get_d3d()->getDeviceContext(), texture.filename.c_str(), nullptr, &texture.dx11_texture, 0);
	}

	if (FAILED(result))
	{
		MessageBox(nullptr, L"Texture loading error", L"ERROR", MB_OK);
		return false;
	}

	m_textures.insert(std::pair(name, texture));
	return true;
}

void graphics::TextureManager::RemoveTexture(const std::string& name)
{
	auto& tex = m_textures.find(name);

	if (tex == m_textures.end())
		return;

	release_dx11_texture(tex->second);
	m_textures.erase(tex);
}

graphics::Texture* graphics::TextureManager::getTexture(const std::string& name)
{
	auto& tex = m_textures.find(name);

	if (tex == m_textures.end())
		return nullptr;

	return &(tex->second);
}

bool graphics::TextureManager::does_file_exist(const wchar_t* fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

void graphics::TextureManager::release_textures()
{
	// release all textures
	for (auto& tex : m_textures)
	{
		release_dx11_texture(tex.second);
	}

	m_textures.clear();
}

void graphics::TextureManager::release_dx11_texture(Texture& texture)
{
	if (texture.dx11_texture)
	{
		texture.dx11_texture->Release();
		texture.dx11_texture = nullptr;
	}
}