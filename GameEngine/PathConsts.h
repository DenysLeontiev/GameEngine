#ifndef PATH_CONSTS_H
#define PATH_CONSTS_H

#include <string>

class PathConsts {
public:
	static inline const std::string DIRECTIONAL_LIGHT_VISUAL_PATH = "assets/shapes/primitives/cube/cube.obj";
	static inline const std::string POINT_LIGHT_VISUAL_PATH = "assets/shapes/primitives/sphere/sphere.obj";
	static inline const std::string SPOT_LIGHT_VISUAL_PATH = "assets/shapes/primitives/cone/cone.obj";

	// Lighting models use these shaders, so they are not affect by itself
	static inline const std::string LIGHT_VERTEX_SHADER_PATH = "assets/shaders/light/light.vert";
	static inline const std::string LIGHT_FRAGMENT_SHADER_PATH = "assets/shaders/light/light.frag";

	static inline const std::string MODEL_VERTEX_SHADER_PATH = "assets/shaders/model/model.vert";
	static inline const std::string MODEL_FRAGMENT_SHADER_PATH = "assets/shaders/model/model.frag";

	static inline const std::string MUSIC_MINECRAFT_PATH = "assets/music/minecraft";
};

#endif