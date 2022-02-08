#pragma once

#include "DX11/DX11VertexBuffer.h"

namespace dx11
{
	class DX11Renderer;
}

namespace MeshBuilder
{

	dx11::DX11VertexBuffer<dx11::VertexType>* CubeMesh(dx11::DX11Renderer* renderer);

}