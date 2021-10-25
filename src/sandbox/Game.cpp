#include "stdafx.hpp"
#include "Game.h"
#include "imGUI/imgui.h"
#include "core/Window.h"
#include "graphics/TextureManager.h"

#include "GravitySolverBruteForce.h"

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
	m_renderer->set_vsync(true);

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

	m_gSolver = new GravitySolverBruteForce();

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

bool Game::Update(const float frame_time)
{
	t += frame_time;
	if (t > 0.25f)
	{
		m_fps = 1.f / frame_time;
		std::cout << "fps: " << m_fps << std::endl;
		t = 0;

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
	camera.handleInput(frame_time);

	m_gSolver->CalculateForces(m_objects);
	for (auto& obj : m_objects)
	{
		obj.Update(m_timestep);
	}

	return true;
}

bool Game::Render()
{
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

	dx11::DX11ShaderData* shader_data = new dx11::DX11ShaderData();
	shader_data->vs_data[0].enable = true;

	BuffType* bufftype = (BuffType*)shader_data->vs_data[0].buffer;
	bufftype->world = glm::transpose(glm::mat4x4(1.0f));
	bufftype->view = glm::transpose(camera.getViewMatrix());
	bufftype->projection = glm::transpose(m_renderer->getProjectionMatrix(45));

	memcpy(shader_data->vs_data[0].buffer, bufftype, sizeof(BuffType));

	m_renderer->begin_frame();
	m_renderer->setTargetBackBuffer();

	ImGui::Text("steps per second: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(m_fps).c_str());

	ImGui::Text("object count: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(m_objects.size()).c_str());

	ImGui::Text("Potential Energy: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(EnergyP).c_str());

	ImGui::Text("Kinetic Energy: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(EnergyK).c_str());

	ImGui::Text("Total Energy: ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(EnergyTotal).c_str());

	m_renderer->set_shader(m_shader);

	for (auto& obj : m_objects)
	{
		obj.SetVertexBuff(m_vbuffer);
		obj.Render(m_renderer, &camera);
	}

	// m_renderer->send_data(shader_data);
	// m_renderer->setTexture(m_textureManager->getTexture("bunny"), 0);
	// m_renderer->render(m_vbuffer);

	m_renderer->end_frame();

	//delete bufftype;
	delete shader_data;
	return true;
}

void Game::InitWindow()
{
	core::Window::WindowConfig config;
	config.borderless = true;
	config.width = 0;
	config.height = 0;
	m_window = new core::Window(&m_input, config);
}

void Game::GenerateTerrainMesh(int resolution)
{
	size_t index_count = resolution * resolution * 6;
	size_t vertex_count = (resolution + 1) * (resolution + 1);
	dx11::VertexType* vertex_buff = new dx11::VertexType[vertex_count];
	dx11::IndexType* index_buff = new dx11::IndexType[index_count];

	int index;
	float positionX, positionZ, u, v, increment;

	// Calculate the number of vertices in the terrain mesh.
	vertex_count = (resolution + 1) * (resolution + 1);

	//indexCount = vertexCount;
	index_count = resolution * resolution * 6;

	// UV coords.
	u = 0;
	v = 0;
	increment = 10 / (float)resolution;
	int v_resolution = resolution + 1;

	// create the vertex array
	for (int i = 0; i < v_resolution; i++)
	{
		for (int j = 0; j < v_resolution; j++)
		{
			// build vertex
			positionX = ((float)j * increment) - ((float)10 / 2);
			positionZ = ((float)i * increment) - ((float)10 / 2);

			vertex_buff[(v_resolution * i) + j].position[0] = positionX;
			vertex_buff[(v_resolution * i) + j].position[1] = 0.0f;
			vertex_buff[(v_resolution * i) + j].position[2] = positionZ;

			vertex_buff[(v_resolution * i) + j].texture[0] = (float)i / ((float)resolution / 64.0f);
			vertex_buff[(v_resolution * i) + j].texture[1] = (float)j / ((float)resolution / 64.0f);

			vertex_buff[(v_resolution * i) + j].normal[0] = 0.0;
			vertex_buff[(v_resolution * i) + j].normal[1] = 1.0;
			vertex_buff[(v_resolution * i) + j].normal[2] = 0.0;
		}
	}

	index = 0;

	// create the index array
	int balancer = resolution % 2;
	for (int i = 0; i < resolution; i++)
	{
		for (int j = 0; j < resolution; j++)
		{
			int k = ((resolution * i) + j) + (resolution % 2);

			if (balancer == 0)
				if (i % 2 == 1)
					k += 1;

			int quad_index = ((v_resolution * i) + j);

			if (k % 2 == 0)
			{
				index_buff[index] = quad_index + v_resolution;
				index_buff[index + 1] = quad_index;
				index_buff[index + 2] = quad_index + 1;

				index_buff[index + 3] = quad_index + v_resolution;
				index_buff[index + 4] = quad_index + 1;
				index_buff[index + 5] = quad_index + v_resolution + 1;
			}
			else // k%2 == 1
			{
				index_buff[index] = quad_index + v_resolution;
				index_buff[index + 1] = quad_index;
				index_buff[index + 2] = quad_index + v_resolution + 1;

				index_buff[index + 3] = quad_index + v_resolution + 1;
				index_buff[index + 4] = quad_index;
				index_buff[index + 5] = quad_index + 1;
			}

			index += 6;
		}
	}

	m_vbuffer->init(m_renderer, vertex_buff, vertex_count, index_buff, index_count);

	delete[] vertex_buff;
	delete[] index_buff;
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
	int count_x = 2;
	int count_y = 1;
	int count_z = 1;

	//for (int x = 0; x < count_x; x++)
	//{
	//	for (int y = 0; y < count_y; y++)
	//	{
	//		for (int z = 0; z < count_z; z++)
	//		{
	//			m_objects.emplace_back(m_textureManager);
	//			m_objects[m_objects.size() - 1].SetPosition({ x,y,z });
	//			m_objects[m_objects.size() - 1].SetMass(1.f);
	//		}
	//	}
	//}

	m_objects.emplace_back(m_textureManager);
	m_objects.emplace_back(m_textureManager);

	m_objects[0].SetPosition({ 1,0,0 });
	m_objects[1].SetPosition({ -1,0,0 });
	m_objects[0].SetMass(1.f);
	m_objects[1].SetMass(1.f);
}

float Game::SystemKineticEnergy()
{
	float e = 0.f;
	for (const auto& obj : m_objects)
	{
		e += obj.KineticEnergy();
	}
	return e / m_timestep;
}

float Game::SystemPotentialEnergy()
{
	float e = 0.f;
	for (int i = 0; i < m_objects.size(); i++)
	{
		for (int j = i + 1; j < m_objects.size(); j++)
		{
			e += m_objects[i].PotentialEnergy(m_objects[j], 1);
		}
	}

	return e;
}