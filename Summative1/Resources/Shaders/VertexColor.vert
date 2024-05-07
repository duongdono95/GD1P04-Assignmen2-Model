#version 460 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;

out vec3 FragColor;
uniform mat4 MVP;
void main() 
{
    gl_Position = MVP * vec4(Position, 1.0f);
    FragColor = Color;
}
