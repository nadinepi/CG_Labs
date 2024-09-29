#version 410

out vec4 frag_color;

in VS_OUT {
	vec3 texcoord;
} fs_in;

uniform samplerCube skybox;

void main()
{
	frag_color = texture(skybox, fs_in.texcoord);
}
