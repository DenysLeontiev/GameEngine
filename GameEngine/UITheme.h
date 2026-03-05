#ifndef UI_THEME_h
#define UI_THEME_H

#include <string>

enum class Theme {
	DarkMode = 0,
	LightMode = 1,
	ClassicMode = 2,

	THEME_COUNT = 3
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
			case Theme::DarkMode:
				return "Dark";
			case Theme::LightMode:
				return "Light";
			case Theme::ClassicMode:
				return "Classic";
			default:
				return "Classic";
		}
	}

	static Theme StringToTheme(std::string theme) {
		if (theme == ThemeToString(Theme::DarkMode)) {
			return Theme::DarkMode;
		}
		else if (theme == ThemeToString(Theme::LightMode)) {
			return Theme::LightMode;
		}
		else if (theme == ThemeToString(Theme::ClassicMode)) {
			return Theme::ClassicMode;
		}

		return Theme::DarkMode;
	}

private:
	std::string availableThemes[static_cast<int>(Theme::THEME_COUNT)] { ThemeToString(Theme::DarkMode), ThemeToString(Theme::LightMode), ThemeToString(Theme::ClassicMode)};

	Theme currentTheme;
};

#endif