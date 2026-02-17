#ifndef UI_THEME_h
#define UI_THEME_H

#include <string>

enum Theme {
	Dark = 0,
	Light = 1,
	Classic = 2,

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
			case Theme::Dark:
				return "Dark";
			case Theme::Light:
				return "Light";
			case Theme::Classic:
				return "Classic";
		}
	}

	static Theme StringToTheme(std::string theme) {
		if (theme == ThemeToString(Theme::Dark)) {
			return Theme::Dark;
		}
		else if (theme == ThemeToString(Theme::Light)) {
			return Theme::Light;
		}
		else if (theme == ThemeToString(Theme::Classic)) {
			return Theme::Classic;
		}

		return Theme::Dark;
	}

private:

	std::string availableThemes[THEME_COUNT] { ThemeToString(Theme::Dark), ThemeToString(Theme::Light), ThemeToString(Theme::Classic)};

	Theme currentTheme;
};

#endif