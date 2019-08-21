//#version 450 core
//
//// Interpolated values from the vertex shaders
//in vec2 UV;
//
// Values that stay constant for the whole mesh.
//
//// Color out.
//out vec3 color;
//
//void main()
//{
//	color =  vec3(UV, 0.0);
//}
#version 330 core

in vec2 UV;
uniform sampler2D TextureSampler;
out vec3 color;

void main()
{
  color = vec3(UV,0);
}