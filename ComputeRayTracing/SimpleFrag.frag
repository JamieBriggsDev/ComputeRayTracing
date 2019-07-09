#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D TextureSampler;

void main(){

	// Output color = color of the texture at the specified UV
	color = vec3(0.0f, 0.0f, 0.0f);
    //color = texture( TextureSampler, UV ).rgb;
} 