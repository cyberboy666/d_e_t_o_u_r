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

uniform float u_x0;
uniform float u_x1;
uniform float u_x2;
uniform float u_x3;
uniform float u_x4;


vec4 mixBlend(vec4 texColour0, vec4 texColour1) {
    vec4 colour;    
    colour = texColour0;
    colour.xyz = u_x0 * texColour0.xyz + (1.0 - u_x0) * texColour1.xyz;

    return colour;
}

vec4 rotate(sampler2D tex, vec2 pos){
    vec4 texColourRotate;
    vec2 center;
    //center = vec2(u_x3 / u_resolution.x, u_x4 / u_resolution.y);
    center = vec2(u_x4, 0.5);

    float r = distance(center, pos);
    float a = atan(pos.x - center.x, pos.y - center.y);

    pos.x = r * cos(a + 2.0 * 3.141592 * u_x3) + 0.5;
    pos.y = r * sin(a + 2.0 * 3.141592 * u_x3) + 0.5;
    
    if((pos.x < 0.0)||(pos.y < 0.0)||(pos.x > 1.0)||(pos.y > 1.0)){
        texColourRotate = vec4(0.0);
    }
    else{
        texColourRotate = texture2D(tex, pos);
    }
    return texColourRotate;
}

void main() {

    vec2 pos = v_texcoord;
    vec4 texColour0;
    vec4 texColour1;


        texColour0 = rotate(u_tex0, v_texcoord);
        texColour1 = rotate(u_tex1, v_texcoord);



    vec4 colour;

    colour = mixBlend(texColour0, texColour1);


    gl_FragColor = colour; 

}
