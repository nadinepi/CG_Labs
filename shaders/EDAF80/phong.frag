#version 410

uniform sampler2D phong_texture;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

in VS_OUT {
	vec2 texcoord;
	vec3 fN;
	vec3 fL;
	vec3 fV;
} fs_in;

out vec4 frag_color;

void main()
{	
	vec3 N = normalize(fs_in.fN);
	vec3 L = normalize(fs_in.fL);
	vec3 V = normalize(fs_in.fV);
	vec3 R = normalize(reflect(-L, N));
	vec3 diffuse =  diffuse * max(dot(N, L), 0.0);
	vec3 specular = specular * pow(max(dot(R, V), 0.0), shininess);
	frag_color.xyz = ambient + diffuse + specular + texture(phong_texture, fs_in.texcoord).xyz;
	frag_color.w = 1.0;
}
