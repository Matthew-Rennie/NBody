#pragma once

#include <string>
#include "d3d11.h"
#include "dx11/DX11_Renderer.h"
#include <vector>

namespace core { class BaseRenderer; };

namespace graphics
{
	struct Texture
	{
		std::string name;
		std::wstring filename;
		ID3D11ShaderResourceView* dx11_texture = nullptr;
	};

	class TextureManager
	{
	public:

		

		// this will reload all textures for the new renderer
		void SetRenderer(core::BaseRenderer* renderer);

		void AddTexture(const std::wstring& filename, const std::string& name);

		void RemoveTexture(const std::wstring& filename);
		void RemoveTexture(const std::string& name);

		Texture* getTexture(const std::string& name);

	private:

		core::BaseRenderer* m_renderer = nullptr;
		
		bool does_file_exist(const wchar_t* fileName);

		void release_textures();
		
		void init_dx11();
		void load_dx11_texture(Texture& texture);
		void release_dx11_texture(Texture& texture);


		std::vector<Texture> m_textures = {};

	};

}