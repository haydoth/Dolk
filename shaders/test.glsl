#vertex
#version 460 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#fragment
#version 460 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0f, 0.5f, 1.0f, 1.0f);
} 