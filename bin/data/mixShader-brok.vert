#version 150

uniform mat4 modelViewProjectionMatrix;

attribute vec2 texcoord;
varying vec2 v_texcoord;

in vec4 position;

void main(){
	v_texcoord = texcoord;

	gl_Position = modelViewProjectionMatrix * position;
}
