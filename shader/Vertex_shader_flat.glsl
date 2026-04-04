#version 330

layout(location = 0) in vec3 apos;


uniform mat4 Perspective_mat;
uniform mat4 View_mat;
uniform mat4 Model_mat;


out vec3 Fragpos;

void main ()
{
    gl_Position =  Perspective_mat*View_mat*vec4(apos,1.0f);

    vec4 pos = Model_mat*vec4(apos,1.0f);
    Fragpos = (pos.xyz);
}