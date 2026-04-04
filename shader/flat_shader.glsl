#version 330 core

in vec3 Fragpos; 
out vec4 Frag_Color;

uniform vec3 objcol;

void main ()
{    
    Frag_Color = vec4(objcol,0.3f);
}