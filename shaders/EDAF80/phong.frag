#version 410

uniform sampler2D leather_texture;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;
uniform int use_normal_mapping;

uniform sampler2D leather_normal_map;
uniform sampler2D leather_rough_map;
uniform mat4 normal_model_to_world;
uniform vec3 light_position;
uniform vec3 camera_position;

in VS_OUT {
	vec2 texcoord;
	vec3 fN;
	vec3 fT;
	vec3 fB;
	vec3 world_position;
} fs_in;

out vec4 frag_color;

void main()
{	
	// Normal mapping
	vec3 normal_rgb = texture(leather_normal_map, fs_in.texcoord).xyz;

	vec3 normal = normalize(normal_rgb * 2.0 - 1.0);

	mat3 TBN = mat3(normalize(fs_in.fT), normalize(fs_in.fB), normalize(fs_in.fN));

	if (use_normal_mapping != 0) {
		normal = TBN * normal;
	} else {
		normal = fs_in.fN;
	}

	vec3 N = normalize(normal);
	vec3 L = normalize(light_position - fs_in.world_position);
	vec3 V = normalize(camera_position - fs_in.world_position);
	vec3 R = normalize(reflect(-L, N));
	vec3 diffuse_r = texture(leather_texture, fs_in.texcoord).xyz * max(dot(N, L), 0.0);
	vec3 specular_r = texture(leather_rough_map, fs_in.texcoord).xyz * pow(max(dot(R, V), 0.0), shininess);
	frag_color.xyz = ambient + diffuse_r + specular_r;
	frag_color.w = 1.0;

}
