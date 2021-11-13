#pragma once

#define CHEESEBOARD_DX11_LINE_COLOR_BLACK	::dx11::Color(0.f,0.f,0.f)
#define CHEESEBOARD_DX11_LINE_COLOR_WHITE	::dx11::Color(1.f,1.f,1.f)
#define CHEESEBOARD_DX11_LINE_COLOR_RED		::dx11::Color(1.f,0.f,0.f)
#define CHEESEBOARD_DX11_LINE_COLOR_GREEN	::dx11::Color(0.f,1.f,0.f)
#define CHEESEBOARD_DX11_LINE_COLOR_BLUE	::dx11::Color(0.f,0.f,1.f)
#define CHEESEBOARD_DX11_LINE_COLOR_YELLOW	::dx11::Color(1.f,1.f,0.f)
#define CHEESEBOARD_DX11_LINE_COLOR_AQUA	::dx11::Color(0.f,1.f,1.f)
#define CHEESEBOARD_DX11_LINE_COLOR_PURPLE	::dx11::Color(1.f,0.f,1.f)

namespace dx11
{
	struct Color
	{
		Color() = default;

		Color(float _r, float _g, float _b) :
			r(_r), g(_g), b(_b) {}

		float r = 0.f;
		float g = 0.f;
		float b = 0.f;
	};
}