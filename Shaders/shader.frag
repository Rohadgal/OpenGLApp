#version 330
in vec4 vCol;
in vec2 texCoord;
in vec3 Normal;

out vec4 color;

struct DirectionalLight{
	vec3 colour;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

uniform sampler2D t_Texture;
uniform DirectionalLight directionalLight;

void main(){
	vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;
	float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);
	vec4 diffuseColour = vec4(directionalLight.colour.z, directionalLight.colour.x, directionalLight.colour.y, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;	

	color = texture(t_Texture, texCoord) * (ambientColour + diffuseColour);
}