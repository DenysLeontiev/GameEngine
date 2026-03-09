#version 330 core
	
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float shininess;
};

struct DirectionalLight {
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float cutOff;
	float outerCutOff;
	
	float constant;
	float linear;
	float quadratic; 
};

#define MAX_DIRECTIONAL_LIGHTS 16
#define MAX_POINT_LIGHTS 32
#define MAX_SPOT_LIGHTS 16

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform int numberOfDirectionalLights;
uniform int numberOfPointLights;
uniform int numberOfSpotLights;

uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform Material material;
	
out vec4 FragColor;

in vec2 texCoords;
in vec3 FragmentWorldPosition;
in vec3 Normal;

uniform sampler2D ourTexture;

uniform vec3 viewPosition;

uniform bool applyLights = true;

void main() {

	if(applyLights == false) {
		FragColor = texture(ourTexture, texCoords) * vec4(material.diffuse, 1.0f);
		return;
	}

	vec3 normal = normalize(Normal);
	vec3 viewDirection = normalize(viewPosition - FragmentWorldPosition);
	
	vec3 resultColor = vec3(0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < numberOfDirectionalLights; i++) {
		resultColor += CalculateDirectionalLight(directionalLights[i], normal, viewDirection);
	}
	
	for (int i = 0; i < numberOfPointLights; i++) {
		resultColor += CalculatePointLight(pointLights[i], normal, FragmentWorldPosition, viewDirection);
	}
	
	for(int i = 0; i < numberOfSpotLights; i++) {
		resultColor += CalculateSpotLight(spotLights[i], normal, FragmentWorldPosition, viewDirection);
	}

	FragColor = texture(ourTexture, texCoords) * vec4(resultColor, 1.0f);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {

	vec3 lightDir = normalize(-light.direction);
	vec3 reflectedDir = reflect(-lightDir, normal);
	
	// Ambient
	vec3 ambient = light.ambient * material.ambient;
	
	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// Specular
	float spec = pow(max(dot(viewDir, reflectedDir), 0.0f), material.shininess);
	vec3 specular = light.specular * (spec *  material.specular);

	return (ambient + diffuse +specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflectedDir = reflect(-lightDir, normal);
	
	// Ambient
	vec3 ambient = light.ambient * material.ambient;
	
	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
	// Specular
	float spec = pow(max(dot(viewDir, reflectedDir), 0.0f), material.shininess);
	vec3 specular = light.specular * (spec *  material.specular);
	
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflectedDir = reflect(-lightDir, normal);
	
	// Ambient
	vec3 ambient = light.ambient * material.ambient;
	
	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);
	
	// Specular
	float spec = pow(max(dot(viewDir, reflectedDir), 0.0f), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);
	
	// Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	// SpotLight intensity calculation
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
	
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	
	return (ambient + diffuse + specular);
}