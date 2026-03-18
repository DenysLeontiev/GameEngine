#ifndef UI_THEME_H
#define UI_THEME_H

#include <string>
#include <imgui.h>
#include "UIConsts.h"

enum class Theme {
	Modern = 0,
	DarkMode = 1,
	LightMode = 2,
	ClassicMode = 3,

	THEME_COUNT = 4
};

struct UITheme {
public:

	std::string* GetAvailableThemes() {
		return availableThemes;
	}

	Theme GetCurrentTheme() const {
		return currentTheme;
	}

	void SetCurrentTheme(Theme theme) {
		this->currentTheme = theme;
	}

	static std::string ThemeToString(Theme theme) {
		switch (theme) {
			case Theme::Modern:
				return "Modern";
			case Theme::DarkMode:
				return "Dark";
			case Theme::LightMode:
				return "Light";
			case Theme::ClassicMode:
				return "Classic";
			default:
				return "Modern";
		}
	}

	static Theme StringToTheme(const std::string& theme) {
		if (theme == ThemeToString(Theme::Modern)) {
			return Theme::Modern;
		}
		else if (theme == ThemeToString(Theme::DarkMode)) {
			return Theme::DarkMode;
		}
		else if (theme == ThemeToString(Theme::LightMode)) {
			return Theme::LightMode;
		}
		else if (theme == ThemeToString(Theme::ClassicMode)) {
			return Theme::ClassicMode;
		}

		return Theme::Modern;
	}

	static void ApplyModernShaping() {
		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding    = UIConsts::WINDOW_ROUNDING;
		style.ChildRounding     = UIConsts::CHILD_ROUNDING;
		style.FrameRounding     = UIConsts::FRAME_ROUNDING;
		style.PopupRounding     = UIConsts::POPUP_ROUNDING;
		style.ScrollbarRounding = UIConsts::SCROLLBAR_ROUNDING;
		style.GrabRounding      = UIConsts::GRAB_ROUNDING;
		style.TabRounding       = UIConsts::TAB_ROUNDING;

		style.WindowPadding     = UIConsts::WINDOW_PADDING;
		style.FramePadding      = UIConsts::FRAME_PADDING;
		style.ItemSpacing       = UIConsts::ITEM_SPACING;
		style.ItemInnerSpacing  = UIConsts::ITEM_INNER_SPACING;
		style.IndentSpacing     = UIConsts::INDENT_SPACING;
		style.ScrollbarSize     = UIConsts::SCROLLBAR_SIZE;
		style.GrabMinSize       = UIConsts::GRAB_MIN_SIZE;

		style.WindowBorderSize  = UIConsts::WINDOW_BORDER_SIZE;
		style.FrameBorderSize   = UIConsts::FRAME_BORDER_SIZE;
		style.PopupBorderSize   = UIConsts::POPUP_BORDER_SIZE;
	}

	static void ApplyModernColors() {
		ImVec4* c = ImGui::GetStyle().Colors;

		// Backgrounds
		c[ImGuiCol_WindowBg]           = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
		c[ImGuiCol_ChildBg]            = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
		c[ImGuiCol_PopupBg]            = ImVec4(0.14f, 0.14f, 0.16f, 0.96f);

		// Borders
		c[ImGuiCol_Border]             = ImVec4(0.22f, 0.22f, 0.26f, 1.00f);
		c[ImGuiCol_BorderShadow]       = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		// Frame (input fields, checkboxes)
		c[ImGuiCol_FrameBg]            = ImVec4(0.18f, 0.18f, 0.21f, 1.00f);
		c[ImGuiCol_FrameBgHovered]     = ImVec4(0.24f, 0.24f, 0.28f, 1.00f);
		c[ImGuiCol_FrameBgActive]      = ImVec4(0.28f, 0.28f, 0.33f, 1.00f);

		// Title bar
		c[ImGuiCol_TitleBg]            = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
		c[ImGuiCol_TitleBgActive]      = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
		c[ImGuiCol_TitleBgCollapsed]   = ImVec4(0.10f, 0.10f, 0.12f, 0.75f);

		c[ImGuiCol_MenuBarBg]          = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);

		// Scrollbar
		c[ImGuiCol_ScrollbarBg]        = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
		c[ImGuiCol_ScrollbarGrab]      = ImVec4(0.28f, 0.28f, 0.32f, 1.00f);
		c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.34f, 0.34f, 0.38f, 1.00f);
		c[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.40f, 0.40f, 0.44f, 1.00f);

		// Accent (blue)
		c[ImGuiCol_CheckMark]          = ImVec4(0.33f, 0.55f, 0.90f, 1.00f);
		c[ImGuiCol_SliderGrab]         = ImVec4(0.33f, 0.55f, 0.90f, 0.78f);
		c[ImGuiCol_SliderGrabActive]   = ImVec4(0.40f, 0.62f, 0.95f, 1.00f);

		// Buttons
		c[ImGuiCol_Button]             = ImVec4(0.22f, 0.22f, 0.26f, 1.00f);
		c[ImGuiCol_ButtonHovered]      = ImVec4(0.30f, 0.52f, 0.85f, 0.80f);
		c[ImGuiCol_ButtonActive]       = ImVec4(0.28f, 0.48f, 0.78f, 1.00f);

		// Headers (collapsing headers, selectables)
		c[ImGuiCol_Header]             = ImVec4(0.22f, 0.22f, 0.26f, 1.00f);
		c[ImGuiCol_HeaderHovered]      = ImVec4(0.30f, 0.52f, 0.85f, 0.60f);
		c[ImGuiCol_HeaderActive]       = ImVec4(0.30f, 0.52f, 0.85f, 0.80f);

		// Separators
		c[ImGuiCol_Separator]          = ImVec4(0.22f, 0.22f, 0.26f, 1.00f);
		c[ImGuiCol_SeparatorHovered]   = ImVec4(0.30f, 0.52f, 0.85f, 0.60f);
		c[ImGuiCol_SeparatorActive]    = ImVec4(0.30f, 0.52f, 0.85f, 0.80f);

		// Resize grip
		c[ImGuiCol_ResizeGrip]         = ImVec4(0.30f, 0.52f, 0.85f, 0.25f);
		c[ImGuiCol_ResizeGripHovered]  = ImVec4(0.30f, 0.52f, 0.85f, 0.60f);
		c[ImGuiCol_ResizeGripActive]   = ImVec4(0.30f, 0.52f, 0.85f, 0.80f);

		// Tabs
		c[ImGuiCol_Tab]                = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
		c[ImGuiCol_TabHovered]         = ImVec4(0.30f, 0.52f, 0.85f, 0.60f);
		c[ImGuiCol_TabActive]          = ImVec4(0.22f, 0.42f, 0.72f, 1.00f);
		c[ImGuiCol_TabUnfocused]       = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
		c[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.32f, 0.55f, 1.00f);

		// Docking
		c[ImGuiCol_DockingPreview]     = ImVec4(0.30f, 0.52f, 0.85f, 0.50f);
		c[ImGuiCol_DockingEmptyBg]     = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

		// Text
		c[ImGuiCol_Text]               = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);
		c[ImGuiCol_TextDisabled]       = ImVec4(0.45f, 0.45f, 0.48f, 1.00f);

		c[ImGuiCol_DragDropTarget]     = ImVec4(0.30f, 0.52f, 0.85f, 0.90f);
	}

	static void ApplyModernStyle() {
		ApplyModernColors();
		ApplyModernShaping();
	}

private:
	std::string availableThemes[static_cast<int>(Theme::THEME_COUNT)] {
		ThemeToString(Theme::Modern),
		ThemeToString(Theme::DarkMode),
		ThemeToString(Theme::LightMode),
		ThemeToString(Theme::ClassicMode)
	};

	Theme currentTheme = Theme::Modern;
};

#endif