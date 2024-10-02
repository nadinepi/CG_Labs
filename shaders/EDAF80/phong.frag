#version 410

uniform sampler2D leather_texture;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;
uniform int use_normal_mapping;

uniform sampler2D leather_normal_map;
uniform mat4 normal_model_to_world;

in VS_OUT {
	vec2 texcoord;
	vec3 fN;
	vec3 fL;
	vec3 fV;
	vec3 fT;
	vec3 fB;
} fs_in;

out vec4 frag_color;

void main()
{	
	// Normal mapping
	vec3 normal_rgb = texture(leather_normal_map, fs_in.texcoord).xyz;

	vec3 normal = normalize(normal_rgb * 2.0 - 1.0);

	mat4 TBN = mat4(vec4(fs_in.fT, 0), vec4(fs_in.fB, 0), vec4(fs_in.fN, 0), vec4(0.0, 0.0, 0.0, 1.0));

	if (use_normal_mapping != 0) {
		normal = vec3(normal_model_to_world * TBN * vec4(normal, 0.0));
	} else {
		normal = fs_in.fN;
	}

	vec3 N = normalize(normal);
	vec3 L = normalize(fs_in.fL);
	vec3 V = normalize(fs_in.fV);
	vec3 R = normalize(reflect(-L, N));
	vec3 diffuse =  diffuse * max(dot(N, L), 0.0);
	vec3 specular = specular * pow(max(dot(R, V), 0.0), shininess);
	frag_color.xyz = ambient + diffuse + specular + texture(leather_texture, fs_in.texcoord).xyz;
	frag_color.w = 1.0;

}
