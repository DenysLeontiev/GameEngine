#version 330
	
out vec4 FragColor;

in vec2 texCoords;

uniform vec4 ourColor;
uniform sampler2D ourTexture;

void main() {
	FragColor = texture(ourTexture, texCoords) * ourColor;
}