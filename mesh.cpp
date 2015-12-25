#include "stdinc.h"
#include "graphics.h"
#include "mesh.h"

mesh::mesh()
{
    texId = vbo_data = ibo_data = wtbo_data = vao_data = 0;
    numBones = 0;
    bones = NULL;
    skinned = false;
}

mesh::~mesh()
{
    free();
}

void mesh::newMesh(vertex* vertices, unsigned int numVerts, unsigned int * indices, unsigned int numIndices)
{
    if (vbo_data!=0 || ibo_data!=0 || vao_data!=0) free();
    vbo_data = CreateBuffer(GL_ARRAY_BUFFER, vertices, sizeof(vertex)*numVerts, GL_STATIC_DRAW);
    ibo_data = CreateBuffer(GL_ELEMENT_ARRAY_BUFFER, indices,sizeof(unsigned int)*numIndices,GL_STATIC_DRAW);
}

void mesh::makeSkinned(unsigned int numVerts, weightInfo * weights)
{
    if (vao_data!=0) free();
    skinned = true;
    wtbo_data = CreateBuffer(GL_ARRAY_BUFFER, weights, sizeof(weightInfo)*numVerts, GL_STATIC_DRAW);
}

void mesh::setTextureId(unsigned int texture)
{
    texId = texture;
}

void mesh::ready()
{
    if (vbo_data==0) return;

    glGenVertexArrays(1, &vao_data);
    glBindVertexArray(vao_data);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_data);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) offsetof(vertex, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) offsetof(vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (GLvoid*) offsetof(vertex, tcoords));


    if (skinned && numBones>0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, wtbo_data);

        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, sizeof(weightInfo), (GLvoid*) offsetof(weightInfo, boneIds));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(weightInfo), (GLvoid*) offsetof(weightInfo, weights));
    }
    glBindVertexArray(0);
}

void mesh::free()
{
    if (vbo_data) glDeleteBuffers(1, &vbo_data);
    if (ibo_data) glDeleteBuffers(1, &ibo_data);
    if (wtbo_data) glDeleteBuffers(1, &wtbo_data);
    if (vao_data) glDeleteVertexArrays(1, &vao_data);
    texId = vbo_data = ibo_data = wtbo_data = vao_data = 0;

    if (numBones>0) delete[] bones;
    numBones=0;

    bones = NULL;
    skinned = false;
}

void mesh::render(graphics* gp, glm::mat4 matModel)
{
    if (!gp) return;
    if (!gp->initialized) return;
    if (vao_data == 0 || ibo_data == 0) return;

    glBindVertexArray(vao_data);

    glm::mat4 mvp = gp->projection * gp->view * matModel;
    glm::mat3 mit = glm::transpose(glm::inverse(glm::mat3(matModel)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);


    if (skinned && numBones>0)
    {
        glUseProgram(gp->skinProgram);
        glUniformMatrix4fv(gp->skin_uniform_mat, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix3fv(gp->skin_uniform_mat_mit, 1, GL_FALSE, glm::value_ptr(mit));

        glm::mat4 * bonemats = new glm::mat4[numBones];
        for (unsigned k=0;k<numBones;k++)
            bonemats[k] = bones[k].Node->combinedTransform * bones[k].offset;
        glUniformMatrix4fv(gp->skin_uniform_bonemats, numBones, GL_FALSE, (GLfloat*)& bonemats[0][0][0]);
        delete[] bonemats;

        glUniform1i(gp->skin_uniform_tex, 0); //XX where GL_TEXTUREXX in glActiveTexture
        glUniform1i(gp->skin_uniform_notlit,0);

        glUniform3f(gp->skin_uniform_sundir, gp->sundir.x, gp->sundir.y, gp->sundir.z);
    }
    else
    {
        glUseProgram(gp->program);
        glUniformMatrix4fv(gp->uniform_mat, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix3fv(gp->uniform_mat_mit, 1, GL_FALSE, glm::value_ptr(mit));
        glUniform1i(gp->uniform_tex, 0);
        glUniform2f(gp->uniform_uvdisp, 0.0f, 0.0f);
        glUniform1i(gp->uniform_notlit,0);

        glUniform3f(gp->uniform_sundir, gp->sundir.x, gp->sundir.y, gp->sundir.z);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_data);
    int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size); // Get the size of buffer
    glDrawElements(GL_TRIANGLES, size/sizeof(GLuint), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}


