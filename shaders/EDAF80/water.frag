#version 410

uniform vec3 light_position;
uniform vec3 camera_position;
uniform samplerCube skybox;
uniform sampler2D water_normal;
uniform float elapsed_time_s;

out vec4 frag_color;

in VS_OUT {
    vec3 world_position;
    vec3 normal;
    vec3 T;
    vec3 B;
    vec2 texcoord;
} fs_in;

void main() { 

    //normal mapping
    vec2 texScale = vec2(8.0, 4.0);
    float normalTime = mod(elapsed_time_s, 100.0);
    vec2 normalSpeed = vec2(-0.05, 0.0);

    vec2 normalCoord0 = fs_in.texcoord.xy * texScale + normalTime * normalSpeed;
    vec2 normalCoord1 = fs_in.texcoord.xy * texScale * 2.0 + normalTime * normalSpeed * 4.0;
    vec2 normalCoord2 = fs_in.texcoord.xy * texScale * 4.0 + normalTime * normalSpeed * 8.0;

    vec3 n1 = texture(water_normal, normalCoord0).xyz * 2.0 - 1.0;
    vec3 n2 = texture(water_normal, normalCoord1).xyz * 2.0 - 1.0;
    vec3 n3 = texture(water_normal, normalCoord2).xyz * 2.0 - 1.0;

    vec3 mapped_normal = normalize(n1 + n2 + n3);

    mat3 TBN = mat3(normalize(fs_in.T), normalize(fs_in.B), normalize(fs_in.normal));

    vec3 N = TBN * mapped_normal;

    //depth color
    vec3 V = normalize(camera_position - fs_in.world_position);
    float facing = 1 - max(dot(V, normalize(fs_in.normal)), 0.0);
    vec4 deep = vec4(0.0, 0.0, 0.1, 1.0);
    vec4 shallow = vec4(0.0, 0.5, 0.5, 1.0);
    vec4 depth_color = mix(deep, shallow, facing);

    //reflection color
    vec3 R = reflect(-V, N);
    vec4 reflection_color = texture(skybox, R);

    //fresnel
    float fc = 0.02037;    
    float fresnel_constant = fc + (1 - fc) * pow((1 - dot(V, N)), 5.0);

    //refraction
    float eta = 1.0/1.33;
    vec4 refraction_color = texture(skybox, refract(-V, N, eta));

    frag_color = depth_color + reflection_color * fresnel_constant + refraction_color * (1 - fresnel_constant);
}