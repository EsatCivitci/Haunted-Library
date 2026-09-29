#version 330 core

out vec4 fragColor;

void main(void)
{
    vec3 pathColor = vec3(0.0, 1.0, 0.0); 
    fragColor = vec4(pathColor, 1.0);
}