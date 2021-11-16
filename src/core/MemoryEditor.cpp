#include "stdafx.hpp"
#include "MemoryEditor.h"
#include "extern/imGUI-Memory-Editor/imgui_memory_editor.h"

void core::MemoryEditor::Draw(void* const data, size_t size) const
{
	static ::MemoryEditor editor;
	editor.DrawWindow("Editor##_memory_editor_", data, size);
}