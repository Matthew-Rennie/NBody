#include "stdafx.hpp"
#include "TextureManager.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include <fstream>
#include "core/BaseRenderer.h"

void graphics::TextureManager::SetRenderer(core::BaseRenderer* renderer)
{
	release_textures();
	m_renderer = renderer;
	if (!m_renderer)
	{
		return;
	}

	dx11::DX11_Renderer* dx11_renderer = dynamic_cast<dx11::DX11_Renderer*>(m_renderer);
	if (dx11_renderer)
	{
		init_dx11();
		return;
	}
}

void graphics::TextureManager::AddTexture(const std::wstring& filename, const std::string& name)
{
	Texture texture;
	texture.filename = filename;
	texture.name = name;

	// check for a texture with the same name and remove it
	for (auto it = m_textures.begin(); it != m_textures.end(); it++)
	{
		if (it->name == name)
		{
			release_dx11_texture(*it);
			m_textures.erase(it);
			break;
		}
	}

	// load texture
	load_dx11_texture(texture);

	m_textures.push_back(texture);
}

void graphics::TextureManager::RemoveTexture(const std::wstring& filename)
{
	for (auto it = m_textures.begin(); it != m_textures.end(); it++)
	{
		if (it->filename == filename)
		{
			release_dx11_texture(*it);
			m_textures.erase(it);
		}
	}
}

void graphics::TextureManager::RemoveTexture(const std::string& name)
{
	for (auto it = m_textures.begin(); it != m_textures.end(); it++)
	{
		if (it->name == name)
		{
			release_dx11_texture(*it);
			m_textures.erase(it);
		}
	}
}

graphics::Texture* graphics::TextureManager::getTexture(const std::string& name)
{
	for (auto& tex : m_textures)
	{
		if (tex.name == name)
		{
			return &tex;
		}
	}
	return nullptr;
}

bool graphics::TextureManager::does_file_exist(const wchar_t* fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

void graphics::TextureManager::release_textures()
{
	// release all textures
	for (auto tex : m_textures)
	{
		release_dx11_texture(tex);
	}

	m_textures.clear();
}

void graphics::TextureManager::init_dx11()
{
	for (auto& tex : m_textures)
	{
		load_dx11_texture(tex);
	}
}

void graphics::TextureManager::load_dx11_texture(Texture& texture)
{
	dx11::DX11_Renderer* dx11_renderer = dynamic_cast<dx11::DX11_Renderer*>(m_renderer);
	if (!dx11_renderer)
		return;

	HRESULT result;

	// check if file exists
	if (texture.filename.empty())
	{
		//filename = L"../res/DefaultDiffuse.png";
		MessageBox(nullptr, L"Texture filename does not exist", L"ERROR", MB_OK);
		return;
	}
	// if not set default texture
	if (!does_file_exist(texture.filename.c_str()))
	{
		// change default texture
		//filename = L"../res/DefaultDiffuse.png";
		MessageBox(nullptr, L"Texture filename does not exist", L"ERROR", MB_OK);
		return;
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
	}
	// Load the texture in.
	if (extension == L"dds")
	{
		result = DirectX::CreateDDSTextureFromFile(dx11_renderer->get_d3d()->getDevice(), dx11_renderer->get_d3d()->getDeviceContext(), texture.filename.c_str(), nullptr, &texture.dx11_texture);
	}
	else
	{
		result = DirectX::CreateWICTextureFromFile(dx11_renderer->get_d3d()->getDevice(), dx11_renderer->get_d3d()->getDeviceContext(), texture.filename.c_str(), nullptr, &texture.dx11_texture, 0);
	}

	if (FAILED(result))
	{
		MessageBox(nullptr, L"Texture loading error", L"ERROR", MB_OK);
	}
}

void graphics::TextureManager::release_dx11_texture(Texture& texture)
{
	if (texture.dx11_texture)
	{
		texture.dx11_texture->Release();
		texture.dx11_texture = nullptr;
	}
}