#include "stdafx.hpp"
#include "Game.h"
#include "imGUI/imgui.h"
#include "core/Window.h"
#include "graphics/TextureManager.h"

#include "GravitySolverBruteForce.h"
#include "GravitySolverOctTree.h"

#include "DX11/DX11Renderer.h"
#include "DX11/DX11VertexBuffer.h"
#include "DX11/DX11Shader.h"
bool Game::Init()
{
	if (!BaseApplication::Init())
		return false;

	InitWindow();

	m_renderer = new dx11::DX11Renderer(&m_input);
	m_renderer->init(m_window);
	// m_renderer->set_vsync(true);

	m_shader = new dx11::DX11Shader(m_renderer);
	m_shader->load_VS(L"shader_vs.cso");
	m_shader->load_PS(L"shader_ps.cso");

	m_vbuffer = new dx11::DX11VertexBuffer();

	//GenerateTerrainMesh(5);
	GenerateCubeMesh();

	camera.setInput(&m_input);
	camera.setWindow(m_window);

	m_textureManager = new graphics::TextureManager(m_renderer);
	m_textureManager->AddTexture(L"res/bunny.png", "bunny");

	// m_gSolver = new GravitySolverBruteForce();
	m_gSolver = new GravitySolverOctTree();

	InitObjects();

	return true;
}

bool Game::Release()
{
	if (!BaseApplication::Release())
		return false;

	SAFE_DELETE(m_renderer);
	SAFE_DELETE(m_shader);
	SAFE_DELETE(m_vbuffer);
	SAFE_DELETE(m_window);
	SAFE_DELETE(m_textureManager);
	SAFE_DELETE(m_gSolver);

	return true;
}

bool Game::Update(const double frame_time)
{
	m_frameClock += frame_time;
	m_guiUpdateClock += frame_time;
	if (m_guiUpdateClock > m_guiUpdateRate)
	{
		m_fps = 1.f / frame_time;
		std::cout << "fps: " << m_fps << std::endl;
		m_guiUpdateClock = 0;

		EnergyP = SystemPotentialEnergy();
		EnergyK = SystemKineticEnergy();
		EnergyTotal = EnergyK + EnergyP;
	}

	if (!BaseApplication::Update(frame_time))
		return false;

	if (m_input.isKeyDown(VK_ESCAPE))
	{
		return false;
	}
	camera.handleInput((float)frame_time);

	m_gSolver->CalculateForces(m_objects);
	for (auto& obj : m_objects)
	{
		obj.Update(m_timestep);
	}

	return true;
}

bool Game::Render()
{
	m_frameClock = 0.0;

	if (!BaseApplication::Render())
		return false;

	camera.update();

	struct BuffType
	{
		glm::mat4x4 world;
		glm::mat4x4 view;
		glm::mat4x4 projection;
	};
	static_assert(sizeof(BuffType) <= 512);

	m_renderer->begin_frame();
	m_renderer->setTargetBackBuffer();

	RenderGUI();

	if (m_enableRendering)
	{
		m_renderer->set_shader(m_shader);
		for (auto& obj : m_objects)
		{
			obj.SetVertexBuff(m_vbuffer);
			obj.Render(m_renderer, &camera);
		}
	}

	m_renderer->end_frame();

	return true;
}

bool Game::ReadyForRender()
{
	if (BaseApplication::ReadyForRender())
	{
		double t = 1.0 / (double)m_targetFrameRate;
		if (m_frameClock > t)
			return true;
	}

	return false;
}

void Game::InitWindow()
{
	core::Window::WindowConfig config;
	config.borderless = true;
	config.width = 0;
	config.height = 0;
	m_window = new core::Window(&m_input, config);
}

void Game::GenerateCubeMesh()
{
	ID3D11Buffer* vertexBuffer, * indexBuffer;

	int vertexCount;
	int indexCount;
	int resolution = 1;

	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// 6 vertices per quad, res*res is face, times 6 for each face
	vertexCount = ((6 * resolution) * resolution) * 6;

	indexCount = vertexCount;

	// Create the vertex and index array.
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	// Vertex variables
	float yincrement = 2.0f / resolution;
	float xincrement = 2.0f / resolution;
	float ystart = 1.0f;
	float xstart = -1.0f;
	//UV variables
	float txu = 0.0f;
	float txv = 0.0f;
	float txuinc = 1.0f / resolution;	// UV increment
	float txvinc = 1.0f / resolution;
	//Counters
	int v = 0;	// vertex counter
	int i = 0;	// index counter

	//front face

	for (int y = 0; y < resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, -1.0f);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart + xincrement, ystart, -1.0f);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart, ystart, -1.0f);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, -1.0f);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart + xincrement, ystart - yincrement, -1.0f);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart + xincrement, ystart, -1.0f);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			txu += txuinc;
			//ystart -= yincrement;
		}

		ystart -= yincrement;
		xstart = -1;

		txu = 0;
		txv += txvinc;
	}

	txv = 0;

	//back face
	ystart = 1;
	xstart = 1;
	for (int y = 0; y < resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, 1.0f);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart - xincrement, ystart, 1.0f);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart, ystart, 1.0f);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, 1.0f);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart - xincrement, ystart - yincrement, 1.0f);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart - xincrement, ystart, 1.0f);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart -= xincrement;
			//ystart -= yincrement;
			txu += txuinc;
		}

		ystart -= yincrement;
		xstart = 1;

		txu = 0;
		txv += txvinc;
	}

	txv = 0;

	//right face
	ystart = 1;
	xstart = -1;
	for (int y = 0; y < resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(1.0f, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(1.0f, ystart, xstart + xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(1.0f, ystart, xstart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(1.0f, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(1.0f, ystart - yincrement, xstart + xincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(1.0f, ystart, xstart + xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			//ystart -= yincrement;
			txu += txuinc;
		}

		ystart -= yincrement;
		xstart = -1;
		txu = 0;
		txv += txvinc;
	}

	txv = 0;

	//left face
	ystart = 1;
	xstart = 1;
	for (int y = 0; y < resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(-1.0f, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(-1.0f, ystart, xstart - xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(-1.0f, ystart, xstart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(-1.0f, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(-1.0f, ystart - yincrement, xstart - xincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(-1.0f, ystart, xstart - xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart -= xincrement;
			//ystart -= yincrement;
			txu += txuinc;
		}

		ystart -= yincrement;
		xstart = 1;
		txu = 0;
		txv += txvinc;
	}

	txv = 0;

	//top face
	ystart = 1;
	xstart = -1;

	for (int y = 0; y < resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, 1.0f, ystart - yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, 1.0f, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart, 1.0f, ystart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, 1.0f, ystart - yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart + xincrement, 1.0f, ystart - yincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, 1.0f, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			//ystart -= yincrement;
			txu += txuinc;
		}

		ystart -= yincrement;
		xstart = -1;
		txu = 0;
		txv += txvinc;
	}

	txv = 0;

	//bottom face
	ystart = -1;
	xstart = -1;

	for (int y = 0; y < resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, -1.0f, ystart + yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, -1.0f, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart, -1.0f, ystart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, -1.0f, ystart + yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart + xincrement, -1.0f, ystart + yincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, -1.0f, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			//ystart -= yincrement;
			txu += txuinc;
		}

		ystart += yincrement;
		xstart = -1;
		txu = 0;
		txv += txvinc;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	m_renderer->get_d3d()->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	m_renderer->get_d3d()->getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	*m_vbuffer->pVertexBuffer() = vertexBuffer;
	*m_vbuffer->pIndexBuffer() = indexBuffer;
	m_vbuffer->VertexCount() = vertexCount;
	m_vbuffer->IndexCount() = indexCount;
}

void Game::InitObjects()
{
	int count_x = 3;
	int count_y = 1;
	int count_z = 3;

	for (int x = 0; x < count_x; x++)
	{
		for (int y = 0; y < count_y; y++)
		{
			for (int z = 0; z < count_z; z++)
			{
				m_objects.emplace_back(m_textureManager);
				m_objects[m_objects.size() - 1].SetPosition({ x,y,z });
				m_objects[m_objects.size() - 1].SetMass(1.f);
			}
		}
	}
	// m_objects[0].SetMass(5.f);
	//
	// long double ld = 0.0l;
	//
	// m_objects.emplace_back(m_textureManager);
	// m_objects.emplace_back(m_textureManager);
	//
	// m_objects[0].SetPosition({ -2,0,0 });
	// m_objects[1].SetPosition({ -1,0,0 });
	// m_objects[0].SetMass(1.f);
	// m_objects[1].SetMass(9.f);
}	//

double Game::SystemKineticEnergy()
{
	double e = 0.f;
	for (const auto& obj : m_objects)
	{
		e += obj.KineticEnergy();
	}
	return e / m_timestep;
}

double Game::SystemPotentialEnergy()
{
	double e = 0.f;
	for (int i = 0; i < m_objects.size(); i++)
	{
		for (int j = i + 1; j < m_objects.size(); j++)
		{
			e += m_objects[i].PotentialEnergy(m_objects[j], 1);
		}
	}

	return e;
}

void Game::RenderGUI()
{
	if (ImGui::TreeNode("Debug Options"))
	{
		ImGui::Checkbox("Enable Rendering", &m_enableRendering);
		ImGui::InputInt("Target Frame Rate", &m_targetFrameRate);
		ImGui::InputDouble("GUI Refresh Rate", &m_guiUpdateRate);

		ImGui::TreePop();
	}

	ImGui::Spacing();

	ImGui::Text("steps per second: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(m_fps).c_str());

	ImGui::Text("object count: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(m_objects.size()).c_str());

	ImGui::Spacing();

	ImGui::Text("Potential Energy: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(EnergyP).c_str());

	ImGui::Text("Kinetic Energy: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(EnergyK).c_str());

	ImGui::Text("Total Energy: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(EnergyTotal).c_str());
}