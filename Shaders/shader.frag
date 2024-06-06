#version 330
in vec4 vCol;
in vec2 texCoord;

out vec4 color;

struct DirectionalLight{
	vec3 colour;
	float ambientIntensity;
};

uniform sampler2D t_Texture;
uniform DirectionalLight directionalLight;

void main(){
	vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;

	color = texture(t_Texture, texCoord) * ambientColour;
}