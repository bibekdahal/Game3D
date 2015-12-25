#version 400

in vec2 f_texcoord;
in vec3 f_normal;
uniform sampler2D texture_sample;

uniform int notlit;
uniform vec3 sundir;

layout (location = 0) out vec4 FragColor;
void main(void) {
    vec4 vTexColor = texture2D(texture_sample, f_texcoord);

    if (notlit==1) FragColor = vTexColor * vec4(vec3(0.6),0.9);
    else if (notlit==2) FragColor = vTexColor;
    else
    {
        vec3 dir = sundir;//vec3(-1.0, 0.0, 0.0);
        float fDiffuseIntensity = max(0, dot(normalize(f_normal), - dir ));
        FragColor = vTexColor*vec4(vec3(0.45+fDiffuseIntensity), 1.0);
    }

    //FragColor = texture2D(texture_sample, f_texcoord);
}
