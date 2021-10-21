#pragma once

#include <string>
#include <unordered_map>

struct ID3D11ShaderResourceView;

namespace dx11
{
	class DX11Renderer;
}

namespace graphics
{
	struct Texture
	{
		std::wstring filename;
		ID3D11ShaderResourceView* dx11_texture = nullptr;
	};

	class TextureManager
	{
	public:

		TextureManager(dx11::DX11Renderer* renderer);

		bool AddTexture(const std::wstring& filename, const std::string& name);

		void RemoveTexture(const std::string& name);

		Texture* getTexture(const std::string& name);

	private:

		dx11::DX11Renderer* m_renderer = nullptr;

		bool does_file_exist(const wchar_t* fileName);

		void release_textures();
		void release_dx11_texture(Texture& texture);

		std::unordered_map<std::string, Texture> m_textures;
	};
}