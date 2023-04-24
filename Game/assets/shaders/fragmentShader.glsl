#version 460 core

out vec4 FragColor;

in float Height;

void main()
{
    FragColor = vec4(Height, Height, Height, 1.0);
}
