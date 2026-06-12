#vertex
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 MODEL_MATRIX;
uniform mat4 VIEW_MATRIX;

out vec2 texCoords;
out vec3 vertexNormal;

void main()
{
	vertexNormal = aNormal;
    gl_Position = VIEW_MATRIX * MODEL_MATRIX * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#fragment
#version 460 core

in vec2 texCoords;
in vec3 vertexNormal;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexNormal, 1.0);
}



