#vertex
#version 460 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoords;
layout (location = 1) in vec3 aNormal;

uniform mat4 MODEL_MATRIX;
uniform mat4 VIEW_MATRIX;

out vec2 texCoords;
out vec3 vertexNormal;

void main()
{
	vertexNormal = mat3(transpose(inverse(MODEL_MATRIX))) * aNormal; // costly
    	gl_Position = VIEW_MATRIX * MODEL_MATRIX * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#fragment
#version 460 core

in vec2 texCoords;
in vec3 vertexNormal;

uniform float AMBIENT_STRENGTH;
uniform vec3 AMBIENT_COLOR;
uniform vec3 LIGHT_COLOR;

out vec4 FragColor;

void main()
{
	vec3 lightDir = vec3(0.5, 1.0, 0.5);
	vec3 final = mix(AMBIENT_COLOR, LIGHT_COLOR, max(dot(lightDir, vertexNormal), AMBIENT_STRENGTH));
	//vec3 final = mix(AMBIENT_COLOR, LIGHT_COLOR, smoothstep(0.75, 0.80, max(dot(lightDir, vertexNormal), AMBIENT_STRENGTH)));
	FragColor = vec4(final, 1.0);
}



