#version 400


layout (location = 0) in vec3 coord3d;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

out vec2 f_texcoord;
out vec3 f_normal;

uniform mat4 m_transform;
uniform mat3 m_inv_trans;
uniform vec2 uv_disp;

void main(void) {
    gl_Position = m_transform * vec4(coord3d, 1.0);
    f_texcoord = texcoord + uv_disp;
    f_normal = normalize(m_inv_trans * normal);
}


