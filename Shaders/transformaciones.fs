#version 330 core

in vec3 our_color;

out vec4 color;//RGB y transparencia

void main(){

	color = vec4(our_color, 1.0);

}