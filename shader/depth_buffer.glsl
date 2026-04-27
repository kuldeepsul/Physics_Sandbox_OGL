#version 330 core

in vec3 Fragpos; 
out vec4 Frag_Color;

uniform vec3 objcol;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main ()
{    
    float depthdata = LinearizeDepth(gl_FragCoord.z) / far;
    Frag_Color = vec4(vec3(depthdata),1.0f);
}