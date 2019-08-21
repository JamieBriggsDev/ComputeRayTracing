#version 450 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;


void main()
{
	UV = vertexUV;
	gl_Position = vec4(vertexPosition_modelspace.xy, 0.0, 1.0);
}

