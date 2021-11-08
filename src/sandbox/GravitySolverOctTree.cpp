#include "stdafx.hpp"
#include "GravitySolverOctTree.h"
#include "OctTreeNode.h"
#include "graphics/WireCubeRenderer.h"
#include "imGUI/imgui.h"

GravitySolverOctTree::~GravitySolverOctTree()
{
	SAFE_DELETE(root);
}

bool GravitySolverOctTree::CalculateForces(std::vector<Object3d>& objects)
{
	SAFE_DELETE(root);
	root = BuildTree(objects);
	root->CalculateMass();
	root->CalculateForces();
	return true;
}

void GravitySolverOctTree::RenderWireframe(graphics::WireCubeRenderer* renderer) const
{
	if (root && renderer)
		RenderNode(root, renderer);
}

void GravitySolverOctTree::DrawDebugGui()
{
	auto s = "OctTreeDebugger##" + std::to_string(count);

	if (root)
	{
		if (ImGui::CollapsingHeader(s.c_str()))
		{
			ImGui::Indent(10);

			DrawDebugGuiNode(root);

			ImGui::Indent(-10);
		}
	}
}

OctTreeNode* GravitySolverOctTree::BuildTree(std::vector<Object3d>& objects)
{
	count++;

	if (objects.size() < 2)
		return nullptr;

	OctTreeNode* root = new OctTreeNode();

	root->m_centrePoint = { 0.0,0.0,0.0 };

	for (auto& c : objects)
		root->m_objects.push_back(&c);

	double longestAxis = 0.0;

	auto longestAxisLambda = [&](double value)
	{
		double v = fabs(value);
		if (v > longestAxis)
			longestAxis = v;
	};

	for (const auto& o : objects)
	{
		longestAxisLambda(o.Position().x);
		longestAxisLambda(o.Position().y);
		longestAxisLambda(o.Position().z);
	}

	// longestAxis = floor(longestAxis + 1.0);

	root->m_lenPointToEdge = longestAxis;

	root->SubDivide();
	return root;
}

void GravitySolverOctTree::RenderNode(const OctTreeNode* node, graphics::WireCubeRenderer* renderer) const
{
	if (node->m_children[0])
	{
		for (auto& c : node->m_children)
		{
			RenderNode(c, renderer);
		}
	}
	else
	{
		renderer->AddCube(node->m_centrePoint, node->m_lenPointToEdge);
	}
}

void GravitySolverOctTree::DrawDebugGuiNode(OctTreeNode* node)
{
	ImGui::TreePush();

	int i = 0;
	auto makeImGuiTag = [&](std::string s) -> std::string
	{
		std::string return_value = s +
			"##memAddr=" + std::to_string(reinterpret_cast<long>(node)) +
			"_count=" + std::to_string(count) +
			"_i=" + std::to_string(++i);

		return return_value;
	};

	if (node->m_children[0])
	{
		ImGui::Indent(10);
		if (ImGui::CollapsingHeader(makeImGuiTag("NodeData").c_str()))
		{
			ImGui::Text("Position");
			ImGui::Indent(10);
			ImGui::Text(("x: " + std::to_string(node->m_centrePoint.x)).c_str());
			ImGui::Text(("y: " + std::to_string(node->m_centrePoint.y)).c_str());
			ImGui::Text(("z: " + std::to_string(node->m_centrePoint.z)).c_str());
			ImGui::Indent(-10);

			ImGui::Text("Centre of Mass");
			ImGui::Indent(10);
			ImGui::Text(("x: " + std::to_string(node->m_centreOfMass.x)).c_str());
			ImGui::Text(("y: " + std::to_string(node->m_centreOfMass.y)).c_str());
			ImGui::Text(("z: " + std::to_string(node->m_centreOfMass.z)).c_str());
			ImGui::Indent(-10);

			ImGui::Text("Radius: ");
			ImGui::Indent(10);
			ImGui::Text((std::to_string(node->m_lenPointToEdge)).c_str());
			ImGui::Indent(-10);

			ImGui::Text("Mass: ");
			ImGui::Indent(10);
			ImGui::Text((std::to_string(node->m_mass)).c_str());
			ImGui::Indent(-10);
		}
		ImGui::Indent(-10);

		for (auto& c : node->m_children)
		{
			if (c == nullptr) continue;
			if (c->m_children[0] == nullptr && c->m_objects.size() == 0) continue;

			ImGui::Indent(10);
			if (ImGui::CollapsingHeader(makeImGuiTag("Node").c_str()))
				DrawDebugGuiNode(c);
			ImGui::Indent(-10);
		}
	}
	else
	{
		if (node->m_objects.size())
			if (ImGui::CollapsingHeader(makeImGuiTag("Object").c_str()))
			{
				Object3d* obj = node->m_objects[0];

				ImGui::Text(("Mass: " + std::to_string(obj->Mass())).c_str());

				ImGui::Text("Position");
				ImGui::Indent(10);
				ImGui::Text(("x: " + std::to_string(obj->Position().x)).c_str());
				ImGui::Text(("y: " + std::to_string(obj->Position().y)).c_str());
				ImGui::Text(("z: " + std::to_string(obj->Position().z)).c_str());
				ImGui::Indent(-10);

				ImGui::Text("Velocity");
				ImGui::Indent(10);
				ImGui::Text(("x: " + std::to_string(obj->Velocity().x)).c_str());
				ImGui::Text(("y: " + std::to_string(obj->Velocity().y)).c_str());
				ImGui::Text(("z: " + std::to_string(obj->Velocity().z)).c_str());
				ImGui::Indent(-10);

				ImGui::Text("Forces");
				ImGui::Indent(10);
				ImGui::Text(("x: " + std::to_string(obj->LastTickForce().x)).c_str());
				ImGui::Text(("y: " + std::to_string(obj->LastTickForce().y)).c_str());
				ImGui::Text(("z: " + std::to_string(obj->LastTickForce().z)).c_str());
				ImGui::Indent(-10);
			}

		//if (node->m_objects.size())
		//	ImGui::Text("node");
	}

	ImGui::TreePop();
}