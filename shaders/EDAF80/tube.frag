#version 410

uniform float elapsed_time_s;
uniform int ate;

in VS_OUT {
    vec2 texcoord;
    vec3 vertex;
} fs_in;

out vec4 frag_color;


void main() {
    vec4 purple = vec4(0.5, 0.37, 0.92, 1.0);
    vec4 blue = vec4(0.0, 0.11, 0.6, 1.0);
    
    float speed = 5.0 + float(ate) * 0.5; 
    float position = mod(fs_in.vertex.z + (-elapsed_time_s * speed), 2.0) / 2.0;
    frag_color = mix(purple, blue, position);
}