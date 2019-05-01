#version 150
//pro-shader
//written by Tim Caldwell
//this is a basic example of how to effect input textures position
#ifdef GL_ES
precision mediump float;
#endif

out vec4 fragmentColor;

varying vec2 v_texcoord;
uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform vec2 u_resolution;
uniform float u_time;
uniform float u_mix;
uniform float u_x0;
uniform float u_x1;
uniform float u_x2;
uniform float u_x3;

void main()
{
    // gl_FragCoord contains the window relative coordinate for the fragment.
    // we use gl_FragCoord.x position to control the red color value.
    // we use gl_FragCoord.y position to control the green color value.
    // please note that all r, g, b, a values are between 0 and 1.
    
//vec2 pos = v_texcoord;

//vec4 texColour0 = texture2DRect(u_tex0, v_texcoord);
// vec4 texColour1 = texture2D(u_tex1, v_texcoord);
vec4 colour = vec4(0.0,0.0,1.0,1.0);

// if(pos.x > u_x0){colour = texColour0;}
// else {colour = texColour1;}
//colour = texColour0;
colour.r = sin(u_time + u_x0);
//colour.g = texColour0[1];
fragmentColor = colour;


    // float windowWidth = 1024.0;
    // float windowHeight = 768.0;
    
	// float r = gl_FragCoord.x / windowWidth;
	// float g = gl_FragCoord.y / windowHeight;
	// float b = 1.0;
	// float a = 1.0;
	// outputColor = vec4(r, g, b, a);
}