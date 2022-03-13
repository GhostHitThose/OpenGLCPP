#shader vertex
#version 330 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;

out vec4 vColor;

void main() {
	gl_Position = pos;
	vColor = color;
}

#shader fragment
#version 330 core

in vec4 vColor;

out vec4 fColor;

void main() {
	fColor = vColor;
}