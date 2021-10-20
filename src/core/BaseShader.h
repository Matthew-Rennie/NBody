#pragma once

namespace core
{
	// the data here is copied into constant buffers
	struct ShaderDataEntry
	{
		static const size_t buffer_size = 256;
		char buffer[buffer_size] = {};
		bool enable = false;
	};

	// collection of all the constant buffers with a shader
	struct ShaderData
	{
		static const size_t num_slots = 8;
		ShaderDataEntry vs_data[num_slots] = {};
		ShaderDataEntry ps_data[num_slots] = {};
		ShaderDataEntry hs_data[num_slots] = {};
		ShaderDataEntry ds_data[num_slots] = {};
		ShaderDataEntry gs_data[num_slots] = {};
	};

	class BaseShader
	{
	public:

		virtual ~BaseShader() = default;
		virtual void Release() = 0;

		virtual bool load_PS(wchar_t* filename) = 0;
		virtual bool load_VS(wchar_t* filename) = 0;

		ShaderData m_shaderData;
	protected:
	};
}
