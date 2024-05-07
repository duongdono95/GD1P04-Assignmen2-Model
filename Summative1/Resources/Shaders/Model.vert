#version 460 core

// Inputs
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;

// Uniforms
uniform mat4 MVP;

// Outputs to Fragment Shader
out vec2 FragTexCoords;

void main() {
    // Apply the MVP transformation
    gl_Position = MVP * vec4(Position, 1.0f);
    FragTexCoords = TexCoords;
}
