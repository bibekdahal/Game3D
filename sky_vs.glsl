#version 400

layout (location = 0) in vec3 coord3d;

uniform mat4 m_transform;

out vec3 f_texcoord;

void main(void) {
    f_texcoord.xyz = normalize(coord3d.xyz);
    gl_Position = m_transform * vec4(coord3d, 1.0);
}

