#version 460 core

// Inputs from Vertex Shader
in vec2 FragTexCoords;

// Uniforms
uniform sampler2D Texture0;

// Output
out vec4 FinalColor;

void main() {
    // Sample the texture based on the fragment's texture coordinates
    FinalColor = texture(Texture0, FragTexCoords);
}
