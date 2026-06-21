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

out vec4 FragColor;

void main()
{
	vec3 ambientColor = vec3(0.9, 0.2, 0.4);
	vec3 lightColor = vec3(0.0, 0.75, 1.0);
	vec3 lightDir = vec3(0.5, 1.0, 0.5);
	vec3 final = mix(ambientColor, lightColor, max(dot(lightDir, vertexNormal), 0.2));
	FragColor = vec4(final, 1.0);
}



