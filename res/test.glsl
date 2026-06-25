#vertex
#version 460 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoords;
layout (location = 1) in vec3 aNormal;

uniform mat4 MODEL_MATRIX;
uniform mat4 VIEW_MATRIX;
uniform mat4 VIEW_PROJECTION_MATRIX;

out vec3 fragmentPos;
out vec2 texCoords;
out vec3 vertexNormal;

void main()
{
	fragmentPos = vec3(VIEW_MATRIX * vec4(aPos, 1.0));
	vertexNormal = mat3(transpose(inverse(VIEW_MATRIX * MODEL_MATRIX))) * aNormal; // costly, move to cpu
    	gl_Position = VIEW_PROJECTION_MATRIX * MODEL_MATRIX * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#fragment
#version 460 core

in vec3 fragmentPos;
in vec2 texCoords;
in vec3 vertexNormal;

uniform mat4 VIEW_MATRIX;
uniform float TIME;
uniform float SPEC_TINT;
uniform float AMBIENT_STRENGTH;
uniform vec3 AMBIENT_COLOR;
uniform vec3 LIGHT_COLOR;

out vec4 FragColor;

const vec3 WHITE = vec3(1.0, 1.0, 1.0);
const vec3 BLACK = vec3(0.0, 0.0, 0.0);

float sin01(float x) {
      return (sin(x) + 1) * 0.5;
}

vec3 specular(float strength, float shininess, float tint, vec3 viewDir, vec3 reflectDir, vec3 col) {
     return pow(max(dot(viewDir, reflectDir), 0.0), shininess) * mix(WHITE, col, tint) * strength;  
}

vec3 diffuse(vec3 normal, vec3 lightDir, vec3 col) {
     return max(dot(normal, lightDir), 0.0) * col;     
}

vec3 ambient(float strength, vec3 col) {
     return col * strength;
}

void main()
{
	vec3 lightPos = vec3(100, 100, 150);
	vec3 lightViewPos = (vec4(lightPos, 1.0) * VIEW_MATRIX).xyz;

	vec3 lightDir = normalize(lightViewPos - fragmentPos);
	vec3 viewDir = normalize(fragmentPos);

	vec3 reflectDir = reflect(-lightDir, vertexNormal); 

	vec3 amb = ambient(0.33, AMBIENT_COLOR);
	vec3 diff = diffuse(normalize(vertexNormal), lightDir, LIGHT_COLOR);	
	vec3 spec = specular(0.5, 16.0, 0, viewDir, reflectDir, LIGHT_COLOR);

	vec3 objCol = vec3(0.7, 1.0, 0.7);
	vec3 result = (amb + diff + spec) * objCol;
	FragColor = vec4(result, 1.0);
}



