#ifndef UI_CONSTS_H
#define UI_CONSTS_H

#include <imgui.h>

class UIConsts {
public:
	// --- Style: Rounding ---
	static inline const float WINDOW_ROUNDING       = 6.0f;
	static inline const float CHILD_ROUNDING        = 6.0f;
	static inline const float FRAME_ROUNDING        = 4.0f;
	static inline const float POPUP_ROUNDING        = 4.0f;
	static inline const float SCROLLBAR_ROUNDING    = 4.0f;
	static inline const float GRAB_ROUNDING         = 3.0f;
	static inline const float TAB_ROUNDING          = 4.0f;

	// --- Style: Spacing & Padding ---
	static inline const ImVec2 WINDOW_PADDING       = ImVec2(10.0f, 10.0f);
	static inline const ImVec2 FRAME_PADDING        = ImVec2(6.0f, 4.0f);
	static inline const ImVec2 ITEM_SPACING         = ImVec2(8.0f, 6.0f);
	static inline const ImVec2 ITEM_INNER_SPACING   = ImVec2(6.0f, 4.0f);
	static inline const float INDENT_SPACING        = 16.0f;
	static inline const float SCROLLBAR_SIZE        = 12.0f;
	static inline const float GRAB_MIN_SIZE         = 8.0f;

	// --- Style: Borders ---
	static inline const float WINDOW_BORDER_SIZE    = 1.0f;
	static inline const float FRAME_BORDER_SIZE     = 0.0f;
	static inline const float POPUP_BORDER_SIZE     = 1.0f;

	// --- Layout ---
	static inline const float HIERARCHY_FOOTER_HEIGHT   = 210.0f;
	static inline const float HIERARCHY_MIN_LIST_HEIGHT = 40.0f;
	static inline const float RENAME_BUTTON_WIDTH       = 72.0f;
	static inline const float RENAME_INPUT_MIN_WIDTH    = 80.0f;
	static inline const float SECTION_INDENT            = 4.0f;
	static inline const float TASKBAR_SPACING           = 24.0f;
	static inline const float FILE_POPUP_UP_BTN_WIDTH   = 80.0f;

	// --- Editor drag speeds ---
	static inline const float TRANSFORM_STEP            = 0.05f;
	static inline const float LIGHT_COLOR_STEP          = 0.01f;
	static inline const float ATTENUATION_CONSTANT_STEP = 0.01f;
	static inline const float ATTENUATION_LINEAR_STEP   = 0.001f;
	static inline const float ATTENUATION_QUAD_STEP     = 0.0001f;
	static inline const float CONE_ANGLE_STEP           = 0.01f;

	// --- FPS thresholds ---
	static inline const float FPS_GOOD_THRESHOLD    = 55.0f;
	static inline const float FPS_OK_THRESHOLD      = 30.0f;

	// --- Accent Button Colors: Blue (Load) ---
	static inline const ImVec4 ACCENT_BUTTON         = ImVec4(0.20f, 0.45f, 0.78f, 1.00f);
	static inline const ImVec4 ACCENT_BUTTON_HOVERED = ImVec4(0.28f, 0.55f, 0.88f, 1.00f);
	static inline const ImVec4 ACCENT_BUTTON_ACTIVE  = ImVec4(0.18f, 0.40f, 0.70f, 1.00f);

	// --- Danger Button Colors: Red (Delete) ---
	static inline const ImVec4 DANGER_BUTTON         = ImVec4(0.70f, 0.18f, 0.18f, 1.00f);
	static inline const ImVec4 DANGER_BUTTON_HOVERED = ImVec4(0.85f, 0.25f, 0.25f, 1.00f);
	static inline const ImVec4 DANGER_BUTTON_ACTIVE  = ImVec4(0.60f, 0.14f, 0.14f, 1.00f);

	// --- File Popup Button ---
	static inline const ImVec4 SUBTLE_BUTTON         = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);

	// --- Status Colors ---
	static inline const ImVec4 STATUS_GOOD           = ImVec4(0.4f, 0.9f, 0.4f, 1.0f);
	static inline const ImVec4 STATUS_WARNING        = ImVec4(0.9f, 0.85f, 0.3f, 1.0f);
	static inline const ImVec4 STATUS_BAD            = ImVec4(0.9f, 0.3f, 0.3f, 1.0f);
};

#endif
