#version 460 core

layout (location = 0) in vec3 a_Position;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_Transform;  

uniform float u_minHeight;
uniform float u_maxHeight;

out float Height;

float map(float value, float min1, float max1, float min2, float max2) {
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main() {
    Height = map(a_Position.y, u_minHeight, u_maxHeight, 0.0f, 1.0f);
    gl_Position = u_Projection * u_View * u_Transform * vec4(a_Position, 1.0);
}

