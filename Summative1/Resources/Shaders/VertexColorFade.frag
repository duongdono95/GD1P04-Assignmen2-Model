#version 460 core

in vec3 FragColor;

uniform float CurrentTime;
uniform vec3 UniformColor;

out vec4 FinalColor;

void main() 
{
    float blendFactor = (sin(CurrentTime) + 1.0) / 2.0;
    vec3 mixedColor = mix(FragColor, UniformColor, blendFactor);
    FinalColor = vec4(mixedColor, 1.0);
}
