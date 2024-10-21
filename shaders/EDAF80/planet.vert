#version 410

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec3 texcoord;

uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;
uniform float elapsed_time_s;

out VS_OUT {
	vec2 texcoord;
} vs_out;


void main()
{
	vs_out.texcoord = texcoord.xy;

	// change vertex position with time (get closer to camera)
	vec3 vertex_n = vec3(vertex.x, vertex.y, elapsed_time_s * 0.2 + vertex.z);

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex_n, 1.0);
}
