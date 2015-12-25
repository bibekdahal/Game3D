
#version 130
attribute vec3 coord3d;
attribute vec2 texcoord;
attribute vec4 weights;
attribute ivec4 boneids;

varying vec2 f_texcoord;

uniform mat4 m_transform;
uniform mat4 m_bones[100];


void main(void) {
    mat4 bonetransform = m_bones[boneids[0]] * weights[0];
    bonetransform += m_bones[boneids[1]] * weights[1];
    bonetransform += m_bones[boneids[2]] * weights[2];
    bonetransform += m_bones[boneids[3]] * weights[3];

    vec4 pos = bonetransform * vec4(coord3d, 1.0);
    gl_Position = m_transform * pos;
    f_texcoord = texcoord;
}
