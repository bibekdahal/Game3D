
in vec3 f_texcoord;
uniform samplerCube texture_sample;

layout (location = 0) out vec4 FragColor;

uniform vec3 sundir;

void main(void)
{
    FragColor = texture (texture_sample, f_texcoord);

    float day_factor = max(-sundir.y, 0.0);

    /*float dotv = max(dot(f_texcoord, -sundir), 0.0);
    vec4 sun_color = vec4(1.0f, 1.0f, 0.8f, 1.0f);

    float pow_factor = day_factor * 175.0 + 25.0;
    float sun_factor = clamp(pow(dotv, pow_factor), 0.0, 1.0);*/


    //FragColor = texture (texture_sample, f_texcoord) * day_factor;// + sun_color * sun_factor;
    FragColor = vec4(130.0/255.0,160.0/255.0,240.0/255.0,1.0) * day_factor;// + sun_color * sun_factor;


    FragColor = texture (texture_sample, f_texcoord);
}
