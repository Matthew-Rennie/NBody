#include "stdafx.hpp"
#include "Game.h"
#include "imGUI/imgui.h"
#include "core/Window.h"
#include "graphics/TextureManager.h"

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

	m_shader = new dx11::DX11Shader(m_renderer);
	m_shader->load_VS(L"shader_vs.cso");
	m_shader->load_PS(L"shader_ps.cso");

	m_vbuffer = new dx11::DX11VertexBuffer();

	GenerateTerrainMesh(5);

	camera.setInput(&m_input);
	camera.setWindow(m_window);

	m_textureManager = new graphics::TextureManager(m_renderer);
	m_textureManager->AddTexture(L"res/bunny.png", "bunny");

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

	return true;
}

bool Game::Update(const float frame_time)
{
	if (!BaseApplication::Update(frame_time))
		return false;

	if (m_input.isKeyDown(VK_ESCAPE))
	{
		return false;
	}
	camera.handleInput(frame_time);
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

	dx11::ShaderData* shader_data = new dx11::ShaderData();
	shader_data->vs_data[0].enable = true;

	BuffType* bufftype = (BuffType*)shader_data->vs_data[0].buffer;
	bufftype->world = glm::transpose(glm::mat4x4(1.0f));
	bufftype->view = glm::transpose(camera.getViewMatrix());
	bufftype->projection = glm::transpose(m_renderer->getProjectionMatrix(45));

	memcpy(shader_data->vs_data[0].buffer, bufftype, sizeof(BuffType));

	m_renderer->begin_frame();
	m_renderer->setTargetBackBuffer();

	ImGui::Text("Hello World");

	m_renderer->set_shader(m_shader);
	m_renderer->send_data(shader_data);
	m_renderer->setTexture(m_textureManager->getTexture("bunny"), 0);
	m_renderer->render(m_vbuffer);

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