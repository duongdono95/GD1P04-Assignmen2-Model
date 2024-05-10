#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

uniform mat4 MVP;  // Model-View-Projection matrix
uniform float textureScale;
out vec2 TexCoords;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    TexCoords = texCoords * textureScale;
}
