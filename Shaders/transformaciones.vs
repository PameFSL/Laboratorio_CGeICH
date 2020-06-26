#version 330 core

layout (location = 0) in vec3 in_position; //Indice del vertice de la posición
layout (location = 1) in vec3 in_color; //Color del vertice

out vec3 our_color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
	//Matriz modelo vista proyeccion
	gl_Position = projection * view * model * vec4(in_position, 1.0);
	our_color = in_color;
}
