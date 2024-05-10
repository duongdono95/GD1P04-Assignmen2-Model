#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 3) in mat4 instanceMatrix; // Assuming location 3 is the start for instance matrices

uniform mat4 viewProjectionMatrix; // Combined View and Projection Matrix

out vec2 FragTexCoords;

void main() {
    FragTexCoords = texCoords;
    gl_Position = viewProjectionMatrix * instanceMatrix * vec4(position, 1.0);
}
