
#version 400
layout (location = 0) in vec3 coord3d;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in ivec4 boneids;
layout (location = 4) in vec4 weights;

out vec2 f_texcoord;
out vec3 f_normal;

uniform mat4 m_transform;
uniform mat3 m_inv_trans;
uniform mat4 m_bones[100];


void main(void) {
    mat4 bonetransform = m_bones[boneids[0]] * weights[0];
    bonetransform += m_bones[boneids[1]] * weights[1];
    bonetransform += m_bones[boneids[2]] * weights[2];
    bonetransform += m_bones[boneids[3]] * weights[3];

    vec4 pos = bonetransform * vec4(coord3d, 1.0);
    gl_Position = m_transform * pos;
    f_texcoord = texcoord;
    f_normal = normalize(m_inv_trans * normal);
}
