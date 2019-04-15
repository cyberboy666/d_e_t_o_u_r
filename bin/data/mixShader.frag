//pro-shader
//written by Tim Caldwell
//this is a basic example of how to effect input textures position
#ifdef GL_ES
precision mediump float;
#endif

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

void main() {

 vec2 pos = v_texcoord;

 vec4 texColour0 = texture2D(u_tex0, v_texcoord);
 vec4 texColour1 = texture2D(u_tex1, v_texcoord);
 vec4 colour;

 if(pos.x > u_x0){colour = texColour0;}
 else {colour = texColour1;}

 gl_FragColor = colour;

}

// void main(){

// vec2 pos = v_texcoord;

// vec4 texColour0 = texture2D(u_tex0, v_texcoord);
// vec4 texColour1 = texture2D(u_tex1, v_texcoord);
// vec4 colour;

// if(pos.x > u_mix){colour = texColour0;}
// else {colour = texColour1;}

// colour.x = sin(u_time);
// gl_FragColor = colour;


// }
