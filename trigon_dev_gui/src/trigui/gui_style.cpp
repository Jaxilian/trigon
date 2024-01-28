#include "gui.h"
#include "imgui.h"

static ImColor background_color = ImColor(0.1f, 0.1f, 0.1f);
static ImColor background_decor = ImColor(0.0f, 0.0f, 0.0f);

void gui_apply_style() {
	ImGuiStyle& style = ImGui::GetStyle();
	style.TabRounding		= 0.0f;
	style.FrameRounding		= 0.0f;
	style.GrabRounding		= 0.0f;
	style.WindowRounding	= 0.0f;
	style.PopupRounding		= 0.0f;
	style.WindowPadding.x	= 5.0f;
	style.WindowPadding.y	= 5.0f;

	style.Colors[ImGuiCol_WindowBg] = background_color;
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(5.0f, 5.0f, 5.0f);
	style.Colors[ImGuiCol_FrameBg]	= background_decor;
	style.Colors[ImGuiCol_Border]	= ImColor(0.0f, 0.0f, 0.0f);
}