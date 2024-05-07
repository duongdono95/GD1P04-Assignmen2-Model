#version 460 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec2 TexCoords;

//Inputs
uniform mat4 MVP;

// Outputs to Fragment Shader
out vec3 FragColor;
out vec2 FragTexCoords;

void main() 
{
    gl_Position = MVP * vec4(Position, 1.0f);
    FragColor = Color;
    FragTexCoords = TexCoords;
}
