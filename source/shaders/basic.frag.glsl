#version 450

layout(location=0) out vec4 color;

layout(location=0) in FragIn {
    vec4 o_color;
    float o_ambient;
    float o_specular;
    vec3 o_camera_position;
    vec3 o_light_position[3];
    vec3 o_normal;
    vec3 o_frag_pos;
} fragIn;

void main() {
    // Compute specular and light for each given
    // light position
    vec3 ambient = vec3(1.0, 1.0, 1.0) * fragIn.o_ambient;
    vec3 normal = normalize(fragIn.o_normal);
    for (int i = 0; i < 3; i++) {
        // Get direction of light
        int light_n = 0;
        vec3 light_dir = normalize(fragIn.o_light_position[light_n] - fragIn.o_frag_pos);

        // Compute diffuse impact
        float diff = max(dot(normal, light_dir), 0.0);
        // TODO: add color to light (default to plain white)
        vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

        // Compute specular reflection
        vec3 view_dir = normalize(fragIn.o_camera_position - fragIn.o_frag_pos);
        vec3 reflect_dir = reflect(-light_dir, normal);
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 256);
        vec3 specular = fragIn.o_specular * spec  * vec3(1.0, 1.0, 1.0);

        color = vec4((ambient + diffuse + specular) * vec3(1.0, 0.0, 0.0), 1.0);

        break;
    }
    // color = vec4(o_frag.o_light_position[light_n] / 3, 1.0);
}
