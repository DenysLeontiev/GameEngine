#version 330 core
	
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float shininess;
};

struct Light {
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform Light light;	
	
out vec4 FragColor;

in vec2 texCoords;
in vec3 FragmentWorldPosition;
in vec3 Normal;

uniform sampler2D ourTexture;

uniform vec3 viewPosition;

void main() {

	// Ambient lightning
	vec3 ambient = light.ambient * material.ambient;
	
	// Diffuse lightning
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragmentWorldPosition); // pointing from fragment to lightPos
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);
	
	// Specular lighting
	vec3 viewDir = normalize(viewPosition - FragmentWorldPosition);
	vec3 reflectedDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectedDir), 0.0f), material.shininess);
	vec3 specular = light.specular * (spec *  material.specular);
	
	float distance = length(light.position - FragmentWorldPosition);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	vec3 resultColor = (ambient + diffuse + specular);

	FragColor = texture(ourTexture, texCoords) * vec4(resultColor, 1.0f);
}