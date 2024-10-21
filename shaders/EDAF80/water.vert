#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform mat4 vertex_world_to_clip;
uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform float elapsed_time_s;
uniform vec3 light_position;

out VS_OUT {
    vec3 world_position;
    vec3 normal;
    vec3 T;
    vec3 B;
    vec2 texcoord;
} vs_out;

float findWaveHeight(in vec2 pos, in float amplitude, in float speed, in float frequency, in float sharpness, in vec2 direction) {
    return pow(amplitude * sin((frequency * (direction.x * pos.x + direction.y * pos.y) +  speed * elapsed_time_s) * 0.5 + 0.5), sharpness);
}

vec3 findWaveNormal(in vec2 pos, in float amplitude, in float speed, in float frequency, in float sharpness, in vec2 direction) {
    float A = 0.5 * sharpness * frequency * amplitude * pow(sin(((direction.x * pos.x + direction.y * pos.y) * frequency + speed * elapsed_time_s) * 0.5 + 0.5), sharpness - 1.0) * cos((direction.x * pos.x + direction.y * pos.y) * frequency + speed * elapsed_time_s);
    float dx = A * direction.x;
    float dz = A * direction.y;
    return vec3(-1.0 * dx, 1.0, - 1.0 * dz);
}

void main() {

    //wave 1 data
    float amplitude = 1.0;
    float phase_speed = 0.5;
    float frequency = 0.2;
    float sharpness = 2.0;
    vec2 direction = vec2(-1, 0);

    //wave 2 data
    float amplitude2 = .5;
    float phase_speed2 = 1.3;
    float frequency2 = 0.4;
    float sharpness2 = 2.0;
    vec2 direction2 = vec2(-0.7, 0.7);

    //get wave heights
    float height1 = findWaveHeight(vertex.xz, amplitude, phase_speed, frequency, sharpness, direction);
    float height2 = findWaveHeight(vertex.xz, amplitude2, phase_speed2, frequency2, sharpness2, direction2);

    //find new vertex position
    vec3 vertex_n = vec3(vertex.x, height1 + height2, vertex.z);

    //get normals
    vec3 normal_n = findWaveNormal(vertex.xz, amplitude, phase_speed, frequency, sharpness, direction);
    vec3 normal_n_2 = findWaveNormal(vertex.xz, amplitude2, phase_speed2, frequency2, sharpness2, direction2);
    vec3 normal_sum = normal_n + normal_n_2;

    //output
    vs_out.world_position = vec3(vertex_model_to_world * vec4(vertex_n, 1.0));
    vs_out.normal = vec3(normal_model_to_world * vec4(normal_sum, 1.0));
    vs_out.T = vec3(1.0, -1.0 * normal_sum.x, 0.0);
    vs_out.B = vec3(0.0, -1.0 * normal_sum.z, 1.0);
    vs_out.texcoord = texcoord;
    gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex_n, 1.0);

}   